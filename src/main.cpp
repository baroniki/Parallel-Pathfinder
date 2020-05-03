#include <iostream>
#include <string>
#include "../include/maze_solver.h"

int main(int argc, char** argv) {
	if(argc < 2) {
		std::cout << "please specify a type of solver" << std::endl;
		return 1;
	}

	const std::string solver_type = argv[1];

	//MazeSolver solver;

	if(!solver_type.compare("bfs"))
		//solver = new BFSSolver();
	else if(!solver_type.compare("dfs"))
		//solver = new DFSSolver();
	else if(!solver_type.compare("dijkstra"))
		//solver = new DijkstraSolver();
	else if(!solver_type.compare("astar"))
		//solver = new AStarSolver();
	else {
		std::cout << "invalid solver type" << std::endl;
		return 1;
	}
	return 0;
}
