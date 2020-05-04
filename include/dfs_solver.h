#ifndef DFS_SOLVER_H
#define DFS_SOLVER_H

#include <iostream>
#include <vector>
#include <queue>
#include <stack>
#include <iterator>
#include <omp.h>

#include "maze_solver.h"



class DFSSolver : public MazeSolver {
public:
	DFSSolver();
	DFSSolver(const std::string maze_file);
	~DFSSolver();
	void setMaze(const std::string maze_file);
	void solve_serial();
	void solve_parallel();

	void output_solution(std::ofstream &ofile);

private:
	class Node{
		private:
			std::vector<int> adjacencies;
			int level;
			Node* pred;
			bool path;
			bool visited;

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
			void setVisited(){
				visited = true;
			}
			bool getVisited(){
				return visited;
			}
	};

	class DFSMaze {
	public:

		DFSMaze(const std::string maze_file);
		~DFSMaze();

		void writeSolution();
		void outputMaze(std::ofstream &ofile);

		// Data members
		std::vector<Node*> adj_list;
		int rows, cols;

	private:
		void parseMazeFile(std::ifstream &iFile);
		std::vector<char> lineToVector(std::string line);
		void createAdjList();

		// Data members
		std::vector<std::vector<char>> maze_vector;
	};

	DFSMaze * maze;
};

#endif
