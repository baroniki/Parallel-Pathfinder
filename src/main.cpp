#include <iostream>
#include <fstream>
#include <string>
#include <time.h>
#include "../include/maze_solver.h"
#include "../include/bfs_solver.h"

int main(int argc, char** argv) {
	if(argc < 2) {
		std::cout << "please specify a type of solver or use 'all' for all types" << std::endl;
		return 1;
	} else if (argc < 3) {
		std::cout << "please enter a maze file or use 'all' for all mazes" << std::endl;
	}

	const std::string solver_type = argv[1];
	const std::string maze_file = argv[2];

	MazeSolver *solver;

	if(!solver_type.compare("bfs"))
		solver = new BFSSolver(maze_file);
	/*
	else if(!solver_type.compare("dfs"))
		//solver = new DFSSolver();
	else if(!solver_type.compare("dijkstra"))
		//solver = new DijkstraSolver();
	else if(!solver_type.compare("astar"))
		//solver = new AStarSolver();
	*/
	else {
		std::cout << "invalid solver type" << std::endl;
		return 1;
	}

	// setup output file to show results

	std::string ofile_name = "data/outputs/results_" + solver_type + ".txt";
	std::ofstream ofile;
	ofile.open(ofile_name);

	ofile << "Results for " << solver_type << " on maze: " << maze_file << ":\n";

	struct timespec start, stop;
	double serial_time, parallel_time;

	if(clock_gettime(CLOCK_REALTIME, &start) == -1) { perror( "clock gettime" );}

	solver->solve_serial();
	
	if(clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror( "clock gettime" );}
	serial_time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;

	ofile << "serial time: " << serial_time << "\n";


	if(clock_gettime(CLOCK_REALTIME, &start) == -1) { perror( "clock gettime" );}

	solver->solve_parallel();
	
	if(clock_gettime(CLOCK_REALTIME, &stop) == -1) { perror( "clock gettime" );}
	parallel_time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;

	ofile << "parallel time: " << parallel_time << "\n";


	solver->output_solution(ofile);

	ofile.close();

	delete solver;
	return 0;
}
