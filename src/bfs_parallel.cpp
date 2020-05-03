#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <time.h>
#include <queue>
#include <iterator>
#include <omp.h>

using namespace std;

//Node class implementation
class Node{
	private:
		std::vector<int> adjacencies;
		int level;
		Node* pred;
		bool path;

	public:
		Node(){
			level = -1;
			pred = NULL;
			path  = false;
		}
		std::vector<int> getAdj(){
			return adjacencies;
		}
		void addAdj(int index){
			adjacencies.push_back(index);
		}

		int getLevel(){
			return level;
		}
		void setLevel(int level){
			this->level = level;
		}

		void setPred(Node* pred){
			this->pred = pred;
		}
		Node* getPred(){
			return pred;
		}
		void setPath(){
			this->path = true;
		}
		bool onPath(){
			return path;
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

				if (maze[y][x] == ' '){

					//[r_left][c_left] == left room 
					//[r_right][c_right] == right room
					int r_left = (y-1)/2, c_left = (x-2)/2;
					int r_right = (y-1)/2, c_right = (x/2);

					//adding the left room to the adj list of the right room, and vice versa
					//using the equation r*C +c
					nodes.at((r_left*column_index) + c_left)->addAdj((r_right*column_index)+c_right);
					nodes.at((r_right*column_index)+c_right)->addAdj((r_left*column_index) + c_left);
		}
	}
	}
	//loop through each row and column appropriately (up and down)
	for (int y =2; y <r-1; y = y+2){
		for (int x=1; x<c; x=x+2){
				if (maze[y][x] == ' '){
					//[r_up][c_up] == up room
					//[r_down][c_down] == down room
					int r_up = (y-2)/2, c_up = (x-1)/2;
					int r_down = y/2, c_down = (x-1)/2;

					//adding the up room to the adj list of the down room, and vice versa
					//using the equation r*C+c
					nodes.at((r_up*column_index)+ c_up)->addAdj((r_down*column_index)+c_down);
					nodes.at((r_down*column_index)+ c_down)->addAdj((r_up*column_index) +c_up);
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


//Running BFS
void bfs(vector<Node*> &nodes, int start_r, int start_c, int c){
	queue<int> q;
	//setting starting node to the [r c] input
	int starting_node = (start_r*c)+start_c;
	nodes.at(starting_node)->setLevel(0);
	q.push(starting_node);
	int v;

	while (!q.empty()){
		v = q.front();
		q.pop();

		//work with all its neighbors now
		vector<int> neighbors = nodes.at(v)->getAdj();
		int thread_count =neighbors.size();
		if (neighbors.size() == 0){
			thread_count = 1;

		}
		//Parallelism.
		omp_set_num_threads(thread_count);
		#pragma omp parallel for
		for (size_t i=0; i <neighbors.size();i++){
			if (nodes.at(neighbors[i])->getLevel() == -1){


				//Ensuring that the level will stay under 10, although not needed here
				nodes.at(neighbors[i])->setLevel((nodes.at(v)->getLevel()+1)%10);
				nodes.at(neighbors[i])->setPred(nodes.at(v));
				q.push(neighbors[i]);

				//going backwards
				if (nodes.at(neighbors[i])== nodes.at(nodes.size()-1)){
					nodes.at(neighbors[i])->setPath();
					Node* temp1 = nodes.at(neighbors[i]);
					while(temp1 != nodes.at(0)){
						temp1 = temp1->getPred();
						temp1->setPath();
					}
				
				}

			}
		}
	}

}



//Main function
//./bfs_parallel maze
int main (int argc, char *argv[]){

	//having them automatically default in this situation
	int start_r=0, start_c=0;
	string filename = "";

	//commandline checking
	if (argc <2){
		cerr << "Need more arguments\n";
		return 0;
	}
	else if (argc >2){
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
			Node* n = new Node();
			nodes.push_back(n);

	}

	//creates adjaceny lists for all rooms
	nodes = createTree(nodes, maze, line_counter, previous_line_length);


	//TIME FUNCTION HERE 
	// measure the start time here
	struct timespec start, stop;
	double time;




	if(clock_gettime(CLOCK_REALTIME, &start) == -1) { perror( "clock gettime" );}

	//implement BFS
	bfs(nodes, start_r, start_c, c);

	if(clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror( "clock gettime" );}
	time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;
	cout << "Execution time: " << time << endl;

	//replace the white spaces with the path
	addLevels(nodes, maze, r, c);

	//Official cout printout
	for ( int i=0; i < line_counter; i++){
		for (unsigned int j =0; j< previous_line_length; j++){
			cout << maze[i][j];
		}
		cout << "\n";
	}

	//closing iFile
	iFile.close();

	//deleting memory
	for (unsigned int i=0; i <nodes.size();i++){
		delete nodes.at(i);
	}

	return 0;
}