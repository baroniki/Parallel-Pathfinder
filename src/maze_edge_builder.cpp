#include <iostream>
#include <vector>
#include <fstream>
#include <random>

using namespace std;

vector<char> edgeBuilder(string line){
	vector<char> new_line;
	new_line.push_back(line[0]);
	//start looking from the second bit, and then compare the two inbetween. 
	for (unsigned int i=1; i < line.length()-1;i++){
		//if there is spaces inbetween the bits. 
		if (line[i-1] == ' ' && line[i] == ' ' && (line[i+1] == ' '|| line[i+1] == 'X')){
			int random_number = rand() % 9 + 1;
			line[i] = random_number + '0';
		}
		else if (line[i-1] == '+' && line[i] == ' ' && line[i+1] == '+'){
			int random_number = rand() % 9 + 1;
			line[i] = random_number + '0';
		}
		new_line.push_back(line[i]);

	}
	new_line.push_back(line[line.length()-1]);

	return new_line;
}

//argument should be ./maze [maze filename]
int main(int argc, char *argv[]){
	if (argc != 2){
		cerr << "You need to follow this pattern: \n ./maze [filename]\n";
		exit(0);
	}
	//bringing in the file
	string filename = argv[1];

	vector<vector<char>> maze;
	//file error checking
	ifstream iFile;
	iFile.open(filename);
	if (!iFile.is_open()){
			cerr <<"File is not valid\n";
			exit(0);
		}

	string line = "";
	int line_counter = 0;
	//Grab first line and the length to compare
	getline(iFile, line);
	unsigned int previous_line_length = line.length();
	line_counter ++;
	vector<char> first_vector = edgeBuilder(line);
	maze.push_back(first_vector);

	//while loop to find line length and number of lines
	while(getline(iFile,line) && !iFile.eof()){
		//checks if the lines are fine
		if (previous_line_length != line.length()){
			cerr << "Broken file";
			exit(0);
		}
		line_counter ++;
		//add the vector line to maze
		vector<char> vector_line = edgeBuilder(line);
		maze.push_back(vector_line);
			
	}


	filename =  filename.substr(0, filename.length()-4) + "_weighted.txt";
	ofstream cout(filename);

	for ( int i=0; i < line_counter; i++){
			for (unsigned int j =0; j< previous_line_length; j++){
				cout << maze[i][j];
		}
		cout << "\n";
	}




	return 0;
}