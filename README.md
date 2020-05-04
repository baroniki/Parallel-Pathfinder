# Parallel Pathfinder to Solve a Maze
A project by Alex Baronikian, Joses Galdamez, William Gonzalez, Jerson Guiterrez

## Description
Pathfinding is a fundamental component of many important applications in fields like GPS, robotics, communication, and logistics. A maze is a collection of paths from a source to a destination. Creating a reliable and accurate graph is a major aspect of modeling a maze as a graph. A pathfinding algorithm will find the shortest valid path from the source to the destination. Solving a maze is a useful model since it is a simple means which can be expanded to applications in computer science fields like GPS navigation, wireless communications, and autonomous mobile robots. In this project, we will parallelize different pathfinding algorithms to solve a maze and analyze their effectiveness. The algorithms we analyzed are Depth-First Search (DFS), Breadth-First Search (BFS), Dijkstras, and A star.

## To run:
- compile with `make`
- run with `srun -n1 -cX ./run <algorithm> <path/to/maze_file>` where:
- X is number of cores -- 1 to run in serial, 2 for two cores, 4 for four cores etc.
- algorithm is the pathfinding algorithm to use. current options are:
	- bfs
	- dfs
	- dijkstra
	- astar
- path/to/maze_file is txt file which represents the maze. data/mazes/ contains some generated mazes to choose from

An example execution would be:  
`srun -n1 -c4 ./run bfs data/mazes/1024x1024_maze.txt`  
  
Outputs are generated in data/outputs
