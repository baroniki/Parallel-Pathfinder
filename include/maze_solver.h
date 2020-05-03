#ifndef MAZE_SOLVER_H
#define MAZE_SOLVER_H

#include <string>
#include <fstream>

class MazeSolver {
public:
	MazeSolver();
	virtual ~MazeSolver() = 0;
	virtual void setMaze(const std::string maze_file) = 0;
	virtual void output_solution(std::ofstream &ofile) = 0;
	virtual void solve_serial() = 0;
	virtual void solve_parallel() = 0;
};

#endif
