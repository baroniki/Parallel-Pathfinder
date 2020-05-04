#ifndef ASTAR_SOLVER_H
#define ASTER_SOLVER_H

#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <iterator>
#include <omp.h>

#include "maze_solver.h"



class AStarSolver : public MazeSolver {
public:
	AStarSolver();
	AStarSolver(const std::string maze_file);
	~AStarSolver();
	void setMaze(const std::string maze_file);
	void solve_serial();
	void solve_parallel();

	void output_solution(std::ofstream &ofile);

private:
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

	class Neighbor{
	public:
		int node;
		int cost;

		Neighbor(int node, int cost){
			this->node = node;
			this->cost = cost;
		}

	};

	struct LessThan{
		bool operator()( Node *lhs,  Node *rhs) {
			return (lhs->getF() > rhs->getF());
		}
	};

	class AStarMaze {
	public:

		AStarMaze(const std::string maze_file);
		~AStarMaze();

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

	int heuristic(int current_node_index, int goal_node_index);
	AStarMaze * maze;
};

#endif
