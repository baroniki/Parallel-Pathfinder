#include "../include/dijkstra_solver.h"

DijkstraSolver::DijkstraSolver() {
	// Do nothing
}

DijkstraSolver::DijkstraSolver(const std::string maze_file) {
	maze = new DijkstraMaze(maze_file);
}

DijkstraSolver::~DijkstraSolver() {
	delete maze;
}

void DijkstraSolver::setMaze(const std::string maze_file) {
	delete maze;
	maze = new DijkstraMaze(maze_file);
}

void DijkstraSolver::output_solution(std::ofstream &ofile) {
	maze->writeSolution();
	maze->outputMaze(ofile);
}

void DijkstraSolver::solve_serial() {
	std::vector<Node*> *nodes = &(maze->adj_list);
	//setting starting node to the [r c] input
	int starting_node = 0;

	/* Dijkstra initialization */
	nodes->at(starting_node)->setDistance(0);
	nodes->at(starting_node)->setSolution(true);
	nodes->at(starting_node)->setPred_index(starting_node);

	//Grab the neighbors
	std::map<int,int> neighbors = nodes->at(starting_node)->getAdjList();
	std::map<int,int>::iterator it;
	for (it = neighbors.begin(); it != neighbors.end(); ++it){
		nodes->at(it->first)->setDistance(it->second);
		nodes->at(it->first)->setPred_index(starting_node);
	}


	//priority queue to make it easier
	std::priority_queue<Node*, std::vector<Node*>, LessThan> pq;

	//only add the neighbors 
	std::map<int,int>::iterator it2;
	for (it2 = neighbors.begin(); it2 != neighbors.end(); ++it2){
		pq.push(nodes->at(it2->first));
		
	}

	/* Dijkstra */
	while (!pq.empty()){
		//find smallest and delete it from pq
		int index = pq.top()->getI();
		pq.pop();

		//only add the neighbors of each nodes so that the queue finds the smallest distance

		std::map<int, int> neigh = nodes->at(index)->getAdjList();
		std::map<int,int>::iterator it3;
		for (it3 = neigh.begin(); it3 != neigh.end(); ++it3){
			//if it hasnt been seen yet, push it in
			if (!nodes->at(it3->first)->getSolution()){
				pq.push(nodes->at(it3->first));
			}
		}

		if (nodes->at(index)->getDistance() == 999999999)
			break;
		//Node has been seen
		nodes->at(index)->setSolution(true);

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
		for (unsigned int i=0; i <neighbors_.size();i++){
			if (!nodes->at(neighbors_.at(i)->node)->getSolution()){
				if((nodes->at(index)->getDistance() + neighbors_.at(i)->cost) < nodes->at(neighbors_.at(i)->node)->getDistance()){
					nodes->at(neighbors_.at(i)->node)->setDistance((nodes->at(index)->getDistance() + neighbors_.at(i)->cost));
					nodes->at(neighbors_.at(i)->node)->setPred_index(index);
				}
			}
			//backtracking for answer
			if (nodes->at(index)== nodes->at(nodes->size()-1)){
				nodes->at(index)->setPath();
				Node* temp1 = nodes->at(index);
				while(temp1 != nodes->at(0)){
					temp1 = nodes->at(temp1->getPred_index());
					temp1->setPath();
				}
			}
		}
		//delete neighbor memory
		for (unsigned int i=0; i <neighbors_.size();i++){
			delete neighbors_.at(i);
		}
	}
}

void DijkstraSolver::solve_parallel() {
	std::vector<Node*> *nodes = &(maze->adj_list);
	//setting starting node to the [r c] input
	int starting_node = 0;

	/* Dijkstra initialization */
	nodes->at(starting_node)->setDistance(0);
	nodes->at(starting_node)->setSolution(true);
	nodes->at(starting_node)->setPred_index(starting_node);

	//Grab the neighbors
	std::map<int,int> neighbors = nodes->at(starting_node)->getAdjList();
	std::map<int,int>::iterator it;
	for (it = neighbors.begin(); it != neighbors.end(); ++it){
		nodes->at(it->first)->setDistance(it->second);
		nodes->at(it->first)->setPred_index(starting_node);
	}


	//priority queue to make it easier
	std::priority_queue<Node*, std::vector<Node*>, LessThan> pq;

	//only add the neighbors 
	std::map<int,int>::iterator it2;
	for (it2 = neighbors.begin(); it2 != neighbors.end(); ++it2){
		pq.push(nodes->at(it2->first));
		
	}

	/* Dijkstra */
	while (!pq.empty()){
		//find smallest and delete it from pq
		int index = pq.top()->getI();
		pq.pop();

		//only add the neighbors of each nodes so that the queue finds the smallest distance

		std::map<int, int> neigh = nodes->at(index)->getAdjList();
		std::map<int,int>::iterator it3;
		for (it3 = neigh.begin(); it3 != neigh.end(); ++it3){
			//if it hasnt been seen yet, push it in
			if (!nodes->at(it3->first)->getSolution()){
				pq.push(nodes->at(it3->first));
			}
		}

		if (nodes->at(index)->getDistance() == 999999999)
			break;
		//Node has been seen
		nodes->at(index)->setSolution(true);

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
		for (unsigned int i=0; i <neighbors_.size();i++){
			if (!nodes->at(neighbors_.at(i)->node)->getSolution()){
				if((nodes->at(index)->getDistance() + neighbors_.at(i)->cost) < nodes->at(neighbors_.at(i)->node)->getDistance()){
					nodes->at(neighbors_.at(i)->node)->setDistance((nodes->at(index)->getDistance() + neighbors_.at(i)->cost));
					nodes->at(neighbors_.at(i)->node)->setPred_index(index);
				}
			}
			//backtracking for answer
			if (nodes->at(index)== nodes->at(nodes->size()-1)){
				nodes->at(index)->setPath();
				Node* temp1 = nodes->at(index);
				while(temp1 != nodes->at(0)){
					temp1 = nodes->at(temp1->getPred_index());
					temp1->setPath();
				}
			}
		}
		//delete neighbor memory
		for (unsigned int i=0; i <neighbors_.size();i++){
			delete neighbors_.at(i);
		}
	}
}


DijkstraSolver::DijkstraMaze::DijkstraMaze(const std::string maze_file) {
	std::ifstream iFile;
	iFile.open(maze_file);

	if (iFile.is_open()){
		parseMazeFile(iFile);

		createAdjList();
	}

	iFile.close();
}

DijkstraSolver::DijkstraMaze::~DijkstraMaze() {
	for (unsigned int i=0; i < adj_list.size();i++){
		delete adj_list.at(i);
	}
}

void DijkstraSolver::DijkstraMaze::writeSolution() {
	for (unsigned int i=0; i < adj_list.size();i++){
		//finding the converted spots on the graph
		int r_ = 2*(i/rows) + 1;
		int c_ = 2*(i%cols) + 1;
		if (adj_list.at(i)->onPath()){
			maze_vector[r_][c_] = 'X';
		}
	}
}

void DijkstraSolver::DijkstraMaze::outputMaze(std::ofstream &ofile) {
	for(unsigned int i=0; i < maze_vector.size(); i++){
		for(unsigned int j =0; j< maze_vector[i].size(); j++){
			ofile << maze_vector[i][j];
		}
		ofile << "\n";
	}
}

void DijkstraSolver::DijkstraMaze::parseMazeFile(std::ifstream &iFile) {
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

std::vector<char> DijkstraSolver::DijkstraMaze::lineToVector(std::string line) {
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

void DijkstraSolver::DijkstraMaze::createAdjList() {
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
