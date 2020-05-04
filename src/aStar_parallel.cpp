#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <queue>
#include <iterator>
#include <omp.h>
#include <map>

using namespace std;

//Node class Implementation
class Node{
private:
	int index;
	std::map<int,int> map_adjacencies;
	int pred_index;
	int distance;
	bool path;
	//added
	int f;
public:
	Node(int index){
		this->index = index;
		pred_index = -1;
		distance = 999999999;
		f = 999999999;
	}
	std::map<int, int> getAdjList(){
		return map_adjacencies;
	}
	void push_pair(int node, int cost){
		map_adjacencies.insert(std::pair<int,int>(node, cost));
	}

	void setDistance(int distance){
		this->distance = distance;
	}
	int getDistance() const{
		return distance;
	}
	int getPred_index(){
		return pred_index;
	}
	void setPred_index(int n){
		pred_index = n;
	}
	int getI() const{
		return index;
	}
	void printAdjList(){
		std::map<int,int> ::iterator it;
		for (it = map_adjacencies.begin(); it != map_adjacencies.end(); ++it){
			std::cout << "(node, cost): (" << it->first  << "," << it->second <<")" <<std::endl;
		}
	}
	void setPath(){
		this->path = true;
	}
	bool onPath(){
		return path;
	}
	//added
	int getF(){
		return f;
	}
	void setF(int f){
		this->f = f;
	}

};

//This was created to iterate through the neighbors. 
//we use a map to create the neighbor list, but openMP doesn't allow
//iterators in the for loops, but rather sequentially going through
//a variable. This fixes those problems. 
class Neighbor{
public:
	int node;
	int cost;

	Neighbor(int node, int cost){
		this->node = node;
		this->cost = cost;
	}

};

/*Comparator for Priority Queue */
struct LessThan{
	bool operator()( Node *lhs,  Node *rhs) {
		return (lhs->getF() > rhs->getF());
	}
};

//Takes in each line to add them to a vector
vector<char> lineToVector(string line){
	vector<char> temp;
	for (unsigned int i=0; i <line.length(); i++){
		char maze = line[i];
		if (!(isdigit(maze) || maze == '+' || maze == '-' || maze == '|' || maze ==' ' )){
			if (maze != 'S' && maze != 'X'){
				cout << maze << endl;
				cerr << "Walls are incompatible\n";
			exit(0);

			}
		}
		if (i==0){
			if (!(maze == '+' || maze == '|')){
				cerr << "Walls are incompatible\n";
				exit(0);
			}
		}
		if (i== line.length()-1){
			if (!(maze == '+' || maze == '|')){
				cerr << "Walls are incompatible\n";
				exit(0);
			}

		}

		temp.push_back(maze);
	}
	return temp;

}

//creates nodes for each cell and returns the vector containing them
vector<Node*> createTree(vector<Node*> &nodes, vector< vector<char>> maze, int r, int c){

	//representing the actual Column count
	int column_index = (c-1)/2;
	
	//loop through each row and column appropriately  (left and right)
	for (int y=1; y < r; y = y+2){
		for (int x=2; x <c-1;x= x+2){
				//between ASCII 0 and 9
				if (maze[y][x] >=48 && maze[y][x] <=57){

					//[r_left][c_left] == left room 
					//[r_right][c_right] == right room
					int r_left = (y-1)/2, c_left = (x-2)/2;
					int r_right = (y-1)/2, c_right = (x/2);

					//adding the left room to the adj list of the right room, and vice versa
					//using the equation r*C +c
					nodes.at((r_left*column_index) + c_left)->push_pair((r_right*column_index)+c_right, maze[y][x]-'0');
					nodes.at((r_right*column_index)+c_right)->push_pair((r_left*column_index) + c_left, maze[y][x]-'0');

				}

	}
	}
	//loop through each row and column appropriately (up and down)
	for (int y =2; y <r-1; y = y+2){
		for (int x=1; x<c; x=x+2){

				//between ASCII 0 and 9
				if (maze[y][x] >=48 && maze[y][x] <=57){

					//[r_up][c_up] == up room
					//[r_down][c_down] == down room
					int r_up = (y-2)/2, c_up = (x-1)/2;
					int r_down = y/2, c_down = (x-1)/2;

					//adding the up room to the adj list of the down room, and vice versa
					//using the equation r*C+c

					nodes.at((r_up*column_index)+ c_up)->push_pair((r_down*column_index)+c_down, maze[y][x]-'0');
					nodes.at((r_down*column_index)+ c_down)->push_pair((r_up*column_index) +c_up, maze[y][x] -'0');


			}
		}
	}

	return  nodes;
}

//adds levels to the maze
void addLevels(vector<Node*> nodes, vector< vector<char> > &maze, int r, int c){
	for (unsigned int i=0; i < nodes.size();i++){
		//finding the converted spots on the graph
		int r_ = 2*(i/c) + 1;
		int c_ = 2*(i%c) + 1;
			if (nodes.at(i)->onPath()){
			maze[r_][c_] = 'X';

		}
	}

}


//Our heuristic that is based on the Manhattan Distance
int heuristic(int current_node_index, int goal_node_index){
	int current_r = 2*(current_node_index) + 1;
	int current_c = 2*(current_node_index) + 1;
	int goal_r = 2*(goal_node_index) + 1;
	int goal_c = 2*(goal_node_index) + 1;

	int h = abs(current_r - goal_r) + abs(current_c - goal_c);


	return h;
}




//Running A*
void aStar(vector<Node*> &nodes, int start_r, int start_c, int c){
	omp_lock_t writelock;

	omp_init_lock(&writelock);
	//setting starting node to the [r c] input
	int starting_node = (start_r*c)+start_c;

	/* A* initialization */
	nodes.at(starting_node)->setDistance(0);
	nodes.at(starting_node)->setF(heuristic(starting_node,nodes.size()-1));
	nodes.at(starting_node)->setPred_index(starting_node);

	//Grab the neighbors
	map<int,int> neighbors = nodes.at(starting_node)->getAdjList();


	//priority queue to make it easier
	priority_queue<Node*, vector<Node*>, LessThan> pq;

	//push the first one 
	pq.push(nodes.at(starting_node));

	/* A* */
	while (!pq.empty()){

		//find smallest f value and delete it from pq
		int index = pq.top()->getI();
		pq.pop();

		map<int, int> neigh = nodes.at(index)->getAdjList();

			//Check all its neighbors
			map<int,int> neighbors_w = nodes.at(index)->getAdjList();
			int thread_count =neighbors_w.size();
			if (neighbors_w.size() == 0){
				thread_count = 1;

			}
			
			//PARALLELISM
			map<int,int> ::iterator it_w;
			//This is needed for the parallel part. 
			vector<Neighbor*> neighbors_;
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
				int d_u = nodes.at(index)->getDistance();
				int d_v = nodes.at(neighbors_.at(i)->node)->getDistance();
				if(weight+d_u < d_v){
					omp_set_lock(&writelock);
					nodes.at(neighbors_.at(i)->node)->setDistance(weight+ d_u);
					//calculate heuristic. 
					int h = heuristic(neighbors_.at(i)->node, nodes.size()-1);
					nodes.at(neighbors_.at(i)->node)->setF(d_v + h);
					//setting predecessor
					nodes.at(neighbors_.at(i)->node)->setPred_index(index);
					pq.push(nodes.at(neighbors_.at(i)->node));
					omp_unset_lock(&writelock);

					//backtracking answer
								if (nodes.at(neighbors_.at(i)->node) == nodes.at(nodes.size()-1)){
									nodes.at(index)->setPath();
										Node* temp1 = nodes.at(index);
										while(temp1 != nodes.at(0)){
											temp1 = nodes.at(temp1->getPred_index());
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

//Main function
int main (int argc, char *argv[]){
	//having them automatically default in this situation
	int start_r=0, start_c=0;
	string filename = "";

	//commandline checking
	if (argc <2){
		cerr << "Need more arguments\n";
		return 0;
	}
	else if (argc >3){
		cerr << "Too many arguments\n";
		return 0;
	}
	else if (argc == 2){
		filename = argv[1];
	}

	

	//initialize the data structure that we will use 
	vector< vector<char>> maze;


	ifstream iFile;
	iFile.open(filename);
	//check if its an invalid input file
	if (!iFile.is_open()){
		cerr <<"File is not valid\n";
		return 0;
	}


	//line counter
	int line_counter =0;
	string line = "";
	//grabbing r and c from the file
	int r=0, c=0;
	unsigned int previous_line_length=0;

	//Grab first line and the length to compare
	getline(iFile, line);
	previous_line_length = line.length();
	line_counter ++;
	vector<char> first_vector = lineToVector(line);
	maze.push_back(first_vector);

	//while loop to find line length and number of lines
	while(getline(iFile,line) && !iFile.eof()){
		//checks if the lines are fine
		if (previous_line_length != line.length()){
			cerr << "inconsistent row lengths\n";
			return 0;
		}
		line_counter ++;
		//add the vector line to maze
		vector<char> vector_line = lineToVector(line);
		maze.push_back(vector_line);
		
	}


	//Calculating the column and row count
	c = (previous_line_length -1 )/2;
	r = (line_counter -1)/2;
	

	//initializing node vector
	vector<Node*> nodes;

	//Ensuring we create specific Node classes for each type of maze.
	for (int i=0; i < (c*r); i++){
		Node* n =new Node(i);
		nodes.push_back(n);
	}

	//creates adjaceny lists for all rooms
	nodes = createTree(nodes, maze, line_counter, previous_line_length);

	//TIME FUNCTION HERE 
	// measure the start time here
	struct timespec start, stop;
	double time;

	if(clock_gettime(CLOCK_REALTIME, &start) == -1) { perror( "clock gettime" );}

	//implement A*
	aStar(nodes, start_r, start_c, c);

	if(clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror( "clock gettime" );}
	time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
	cout << "Execution time: " << time << endl;


	//replace the white spaces with the corresponding level for the maze
	addLevels(nodes, maze, r, c);


	//Official cout printout for submission
	// for ( int i=0; i < line_counter; i++){
	// 	for (unsigned int j =0; j< previous_line_length; j++){
	// 		//removing 
	// 		if (isdigit(maze[i][j])){
	// 			cout << ' ';
	// 		}
	// 		else{
	// 			cout << maze[i][j];

	// 		}
	// 	}
	// 	cout << "\n";
	// }

	//closing iFile
	iFile.close();

	//deleting memory
	for (unsigned int i=0; i <nodes.size();i++){
		delete nodes.at(i);
	}

	return 0;
}