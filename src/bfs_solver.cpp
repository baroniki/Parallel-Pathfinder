#include "../include/bfs_solver.h"

BFSSolver::BFSSolver() {
	// Do nothing
}

BFSSolver::BFSSolver(const std::string maze_file) {
	maze = new BFSMaze(maze_file);
}

BFSSolver::~BFSSolver() {
	delete maze;
}

void BFSSolver::setMaze(const std::string maze_file) {
	delete maze;
	maze = new BFSMaze(maze_file);
}

void BFSSolver::output_solution(std::ofstream &ofile) {
	maze->writeSolution();
	maze->outputMaze(ofile);
}

void BFSSolver::solve_serial() {
	std::vector<Node*> * nodes = &(maze->adj_list);
	std::queue<int> q;
	//setting starting node to the [r c] input
	int starting_node = 0;
	nodes->at(starting_node)->setLevel(0);
	q.push(starting_node);
	int v;

	while (!q.empty()){
		v = q.front();
		q.pop();

		//work with all its neighbors now
		std::vector<int> neighbors = nodes->at(v)->getAdj();
		for (size_t i=0; i <neighbors.size();i++){
			if (nodes->at(neighbors[i])->getLevel() == -1){
				//Ensuring that the level will stay under 10, although not needed here
				nodes->at(neighbors[i])->setLevel((nodes->at(v)->getLevel()+1)%10);
				nodes->at(neighbors[i])->setPred(nodes->at(v));
				q.push(neighbors[i]);

				//going backwards
				if (nodes->at(neighbors[i])== nodes->at(nodes->size()-1)){
					nodes->at(neighbors[i])->setPath();
					Node* temp1 = nodes->at(neighbors[i]);
					while(temp1 != nodes->at(0)){
						temp1 = temp1->getPred();
						temp1->setPath();
					}
				}
			}
		}
	}
}

void BFSSolver::solve_parallel() {
	omp_lock_t writelock;
	omp_init_lock(&writelock);

	std::vector<Node*> * nodes = &(maze->adj_list);
	std::queue<int> q;
	//setting starting node to the [r c] input
	int starting_node = 0;
	nodes->at(starting_node)->setLevel(0);
	q.push(starting_node);
	int v;

	while (!q.empty()){
		v = q.front();
		q.pop();

		//work with all its neighbors now
		std::vector<int> neighbors = nodes->at(v)->getAdj();
		int thread_count =neighbors.size();
		if (neighbors.size() == 0){
			thread_count = 1;

		}
		//Parallelism.
		omp_set_num_threads(thread_count);
		#pragma omp parallel for
		for (size_t i=0; i <neighbors.size();i++){
			if (nodes->at(neighbors[i])->getLevel() == -1){
				omp_set_lock(&writelock);

				//Ensuring that the level will stay under 10, although not needed here
				nodes->at(neighbors[i])->setLevel((nodes->at(v)->getLevel()+1)%10);
				nodes->at(neighbors[i])->setPred(nodes->at(v));
				q.push(neighbors[i]);

				omp_unset_lock(&writelock);

				//going backwards
				if (nodes->at(neighbors[i])== nodes->at(nodes->size()-1)){
					nodes->at(neighbors[i])->setPath();
					Node* temp1 = nodes->at(neighbors[i]);
					while(temp1 != nodes->at(0)){
						temp1 = temp1->getPred();
						temp1->setPath();
					}
				}
			}
		}
	}

	omp_unset_lock(&writelock);
}


BFSSolver::BFSMaze::BFSMaze(const std::string maze_file) {
	std::ifstream iFile;
	iFile.open(maze_file);

	if (iFile.is_open()){
		parseMazeFile(iFile);

		createAdjList();
	}

	iFile.close();
}

BFSSolver::BFSMaze::~BFSMaze() {
	for (unsigned int i=0; i < adj_list.size();i++){
		delete adj_list.at(i);
	}
}

void BFSSolver::BFSMaze::writeSolution() {
	for (unsigned int i=0; i < adj_list.size();i++){
		//finding the converted spots on the graph
		int r_ = 2*(i/rows) + 1;
		int c_ = 2*(i%cols) + 1;
		if (adj_list.at(i)->onPath()){
			maze_vector[r_][c_] = 'X';
		}
	}
}

void BFSSolver::BFSMaze::outputMaze(std::ofstream &ofile) {
	for(unsigned int i=0; i < maze_vector.size(); i++){
		for(unsigned int j =0; j< maze_vector[i].size(); j++){
			ofile << maze_vector[i][j];
		}
		ofile << "\n";
	}
}

void BFSSolver::BFSMaze::parseMazeFile(std::ifstream &iFile) {
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

std::vector<char> BFSSolver::BFSMaze::lineToVector(std::string line) {
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

void BFSSolver::BFSMaze::createAdjList() {
	for (int i=0; i < (cols*rows); i++){
		Node* n = new Node();
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
				adj_list.at((r_left*column_index) + c_left)->addAdj((r_right*column_index) + c_right);
				adj_list.at((r_right*column_index) + c_right)->addAdj((r_left*column_index) + c_left);
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
				adj_list.at((r_up*column_index) + c_up)->addAdj((r_down*column_index) + c_down);
				adj_list.at((r_down*column_index)+ c_down)->addAdj((r_up*column_index) + c_up);
			}
		}
	}
}
