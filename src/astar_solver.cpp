#include "../include/astar_solver.h"

AStarSolver::AStarSolver() {
	// Do nothing
}

AStarSolver::AStarSolver(const std::string maze_file) {
	maze = new AStarMaze(maze_file);
}

AStarSolver::~AStarSolver() {
	delete maze;
}

void AStarSolver::setMaze(const std::string maze_file) {
	delete maze;
	maze = new AStarMaze(maze_file);
}

void AStarSolver::output_solution(std::ofstream &ofile) {
	maze->writeSolution();
	maze->outputMaze(ofile);
}

int AStarSolver::heuristic(int current_node_index, int goal_node_index){
	int current_r = 2*(current_node_index) + 1;
	int current_c = 2*(current_node_index) + 1;
	int goal_r = 2*(goal_node_index) + 1;
	int goal_c = 2*(goal_node_index) + 1;

	int h = abs(current_r - goal_r) + abs(current_c - goal_c);


	return h;
}

void AStarSolver::solve_serial() {
	std::vector<Node*> *nodes = &(maze->adj_list);
	//setting starting node to the [r c] input
	omp_lock_t writelock;

	omp_init_lock(&writelock);
	//setting starting node to the [r c] input
	int starting_node = 0;

	/* A* initialization */
	nodes->at(starting_node)->setDistance(0);
	nodes->at(starting_node)->setF(heuristic(starting_node,nodes->size()-1));
	nodes->at(starting_node)->setPred_index(starting_node);

	//Grab the neighbors
	std::map<int,int> neighbors = nodes->at(starting_node)->getAdjList();


	//priority queue to make it easier
	std::priority_queue<Node*, std::vector<Node*>, LessThan> pq;

	//push the first one 
	pq.push(nodes->at(starting_node));

	/* A* */
	while (!pq.empty()){

		//find smallest f value and delete it from pq
		int index = pq.top()->getI();
		pq.pop();

		std::map<int, int> neigh = nodes->at(index)->getAdjList();

		//Check all its neighbors
		std::map<int,int> neighbors_w = nodes->at(index)->getAdjList();
		
		//PARALLELISM
		std::map<int,int>::iterator it_w;
		//This is needed for the parallel part. 
		std::vector<Neighbor*> neighbors_;
		for (it_w = neighbors_w.begin(); it_w != neighbors_w.end(); ++it_w){
			Neighbor* temp = new Neighbor(it_w->first, it_w->second);
			neighbors_.push_back(temp);
		}
		//convert to vector in order to use openMP
		omp_set_num_threads(1);
		#pragma omp parallel for
		//examine each edge from each neighbor
		for (unsigned int i=0; i <neighbors_.size();i++){
			int weight = neighbors_.at(i)->cost;
			int d_u = nodes->at(index)->getDistance();
			int d_v = nodes->at(neighbors_.at(i)->node)->getDistance();
			if(weight+d_u < d_v){
				omp_set_lock(&writelock);
				nodes->at(neighbors_.at(i)->node)->setDistance(weight+ d_u);
				//calculate heuristic. 
				int h = heuristic(neighbors_.at(i)->node, nodes->size()-1);
				nodes->at(neighbors_.at(i)->node)->setF(d_v + h);
				//setting predecessor
				nodes->at(neighbors_.at(i)->node)->setPred_index(index);
				pq.push(nodes->at(neighbors_.at(i)->node));
				omp_unset_lock(&writelock);

				//backtracking answer
				if (nodes->at(neighbors_.at(i)->node) == nodes->at(nodes->size()-1)){
					nodes->at(index)->setPath();
					Node* temp1 = nodes->at(index);
					while(temp1 != nodes->at(0)){
						temp1 = nodes->at(temp1->getPred_index());
						temp1->setPath();
					}
				}
			}
		}
		//delete neighbor memory
		for (unsigned int i=0; i <neighbors_.size();i++){
			delete neighbors_.at(i);
		}
	}
	omp_destroy_lock(&writelock);
}

void AStarSolver::solve_parallel() {
	std::vector<Node*> *nodes = &(maze->adj_list);
	//setting starting node to the [r c] input
	omp_lock_t writelock;

	omp_init_lock(&writelock);
	//setting starting node to the [r c] input
	int starting_node = 0;

	/* A* initialization */
	nodes->at(starting_node)->setDistance(0);
	nodes->at(starting_node)->setF(heuristic(starting_node,nodes->size()-1));
	nodes->at(starting_node)->setPred_index(starting_node);

	//Grab the neighbors
	std::map<int,int> neighbors = nodes->at(starting_node)->getAdjList();


	//priority queue to make it easier
	std::priority_queue<Node*, std::vector<Node*>, LessThan> pq;

	//push the first one 
	pq.push(nodes->at(starting_node));

	/* A* */
	while (!pq.empty()){

		//find smallest f value and delete it from pq
		int index = pq.top()->getI();
		pq.pop();

		std::map<int, int> neigh = nodes->at(index)->getAdjList();

		//Check all its neighbors
		std::map<int,int> neighbors_w = nodes->at(index)->getAdjList();
		int thread_count =neighbors_w.size();
		if (neighbors_w.size() == 0){
			thread_count = 1;

		}
		
		//PARALLELISM
		std::map<int,int>::iterator it_w;
		//This is needed for the parallel part. 
		std::vector<Neighbor*> neighbors_;
		for (it_w = neighbors_w.begin(); it_w != neighbors_w.end(); ++it_w){
			Neighbor* temp = new Neighbor(it_w->first, it_w->second);
			neighbors_.push_back(temp);
		}
		//convert to vector in order to use openMP
		omp_set_num_threads(thread_count);
		#pragma omp parallel for
		//examine each edge from each neighbor
		for (unsigned int i=0; i <neighbors_.size();i++){
			int weight = neighbors_.at(i)->cost;
			int d_u = nodes->at(index)->getDistance();
			int d_v = nodes->at(neighbors_.at(i)->node)->getDistance();
			if(weight+d_u < d_v){
				omp_set_lock(&writelock);
				nodes->at(neighbors_.at(i)->node)->setDistance(weight+ d_u);
				//calculate heuristic. 
				int h = heuristic(neighbors_.at(i)->node, nodes->size()-1);
				nodes->at(neighbors_.at(i)->node)->setF(d_v + h);
				//setting predecessor
				nodes->at(neighbors_.at(i)->node)->setPred_index(index);
				pq.push(nodes->at(neighbors_.at(i)->node));
				omp_unset_lock(&writelock);

				//backtracking answer
				if (nodes->at(neighbors_.at(i)->node) == nodes->at(nodes->size()-1)){
					nodes->at(index)->setPath();
					Node* temp1 = nodes->at(index);
					while(temp1 != nodes->at(0)){
						temp1 = nodes->at(temp1->getPred_index());
						temp1->setPath();
					}
				}
			}
		}
		//delete neighbor memory
		for (unsigned int i=0; i <neighbors_.size();i++){
			delete neighbors_.at(i);
		}
	}
	omp_destroy_lock(&writelock);
}


AStarSolver::AStarMaze::AStarMaze(const std::string maze_file) {
	std::ifstream iFile;
	iFile.open(maze_file);

	if (iFile.is_open()){
		parseMazeFile(iFile);

		createAdjList();
	}

	iFile.close();
}

AStarSolver::AStarMaze::~AStarMaze() {
	for (unsigned int i=0; i < adj_list.size();i++){
		delete adj_list.at(i);
	}
}

void AStarSolver::AStarMaze::writeSolution() {
	for (unsigned int i=0; i < adj_list.size();i++){
		//finding the converted spots on the graph
		int r_ = 2*(i/rows) + 1;
		int c_ = 2*(i%cols) + 1;
		if (adj_list.at(i)->onPath()){
			maze_vector[r_][c_] = 'X';
		}
	}
}

void AStarSolver::AStarMaze::outputMaze(std::ofstream &ofile) {
	for(unsigned int i=0; i < maze_vector.size(); i++){
		for(unsigned int j =0; j< maze_vector[i].size(); j++){
			ofile << maze_vector[i][j];
		}
		ofile << "\n";
	}
}

void AStarSolver::AStarMaze::parseMazeFile(std::ifstream &iFile) {
	int line_counter = 0;
	std::string line = "";

	//Grab first line and the length to compare
	getline(iFile, line);
	unsigned int line_length = line.length();
	line_counter++;
	std::vector<char> first_vector = lineToVector(line);
	maze_vector.push_back(first_vector);

	//while loop to find line length and number of lines
	while(getline(iFile,line) && !iFile.eof()){
		//checks if the lines are fine
		if (line_length != line.length()){
			std::cerr << "inconsistent row lengths\n";
			return;
		}
		line_counter++;
		//add the vector line to maze
		std::vector<char> vector_line = lineToVector(line);
		maze_vector.push_back(vector_line);
		
	}

	//Calculating the column and row count
	cols = (line_length-1)/2;
	rows = (line_counter-1)/2;
}

std::vector<char> AStarSolver::AStarMaze::lineToVector(std::string line) {
	std::vector<char> temp;
	for (unsigned int i=0; i <line.length(); i++){
		char maze = line[i];

		if (!(isdigit(maze) || maze == '+' || maze == '-' || maze == '|' || maze ==' ' )){
			if (maze != 'S' && maze != 'X'){
				exit(0);
			}
		}
		if (i==0){
			if (!(maze == '+' || maze == '|')){
				exit(0);
			}
		}
		if (i== line.length()-1){
			if (!(maze == '+' || maze == '|')){
				exit(0);
			}
		}

		temp.push_back(maze);
	}
	return temp;
}

void AStarSolver::AStarMaze::createAdjList() {
	for (int i=0; i < (cols*rows); i++){
		Node* n = new Node(i);
		adj_list.push_back(n);
	}

	//representing the actual Column count
	int column_index = cols;
	
	//loop through each row and column appropriately  (left and right)
	for (unsigned int y=1; y < maze_vector.size(); y = y+2){
		for (unsigned int x=2; x < maze_vector[y].size()-1; x = x+2){
			if (maze_vector[y][x] == ' '){
				//[r_left][c_left] == left room 
				//[r_right][c_right] == right room
				int r_left = (y-1)/2, c_left = (x-2)/2;
				int r_right = (y-1)/2, c_right = (x/2);

				//adding the left room to the adj list of the right room, and vice versa
				//using the equation r*C +c
				adj_list.at((r_left*column_index) + c_left)->push_pair((r_right*column_index)+c_right, maze_vector[y][x]-'0');
				adj_list.at((r_right*column_index)+c_right)->push_pair((r_left*column_index) + c_left, maze_vector[y][x]-'0');
			}
		}
	}
	//loop through each row and column appropriately (up and down)
	for (unsigned int y =2; y <maze_vector.size()-1; y = y+2){
		for (unsigned int x=1; x<maze_vector[y].size(); x=x+2){
			if (maze_vector[y][x] == ' '){
				//[r_up][c_up] == up room
				//[r_down][c_down] == down room
				int r_up = (y-2)/2, c_up = (x-1)/2;
				int r_down = y/2, c_down = (x-1)/2;

				//adding the up room to the adj list of the down room, and vice versa
				//using the equation r*C+c
				adj_list.at((r_up*column_index)+ c_up)->push_pair((r_down*column_index)+c_down, maze_vector[y][x]-'0');
				adj_list.at((r_down*column_index)+ c_down)->push_pair((r_up*column_index) +c_up, maze_vector[y][x] -'0');
			}
		}
	}
}
