#ifndef MAZE_SOLVER_H
#define MAZE_SOLVER_H

#include "maze_tree.h"

class MazeSolver {
public:
	MazeSolver();
	MazeSolver(const std::string maze_file);
	void setMaze(const std::string maze_file);
	virtual void solve() = 0;

protected:
	MazeTree maze;
};

#endif
