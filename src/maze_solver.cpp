#include "../include/maze_solver.h"

MazeSolver::MazeSolver() {
	// do nothing
}

MazeSolver::MazeSolver(const std::string maze_file) {
	maze = MazeTree(maze_file);
}

void MazeSolver::setMaze(const std::string maze_file) {
	maze = MazeTree(maze_file);
}
