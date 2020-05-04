FLAGS = -g -Wall -std=c++11 -fopenmp

all: run

run: maze_solver bfs_solver dfs_solver dijkstra_solver astar_solver src/main.cpp
	g++ $(FLAGS) src/main.cpp bin/maze_solver.o bin/bfs_solver.o bin/dfs_solver.o bin/dijkstra_solver.o bin/astar_solver.o -o run

maze_solver: src/maze_solver.cpp
	g++ $(FLAGS) -c src/maze_solver.cpp -o bin/maze_solver.o

bfs_solver: src/bfs_solver.cpp
	g++ $(FLAGS) -c src/bfs_solver.cpp -o bin/bfs_solver.o

dfs_solver: src/dfs_solver.cpp
	g++ $(FLAGS) -c src/dfs_solver.cpp -o bin/dfs_solver.o

dijkstra_solver: src/dijkstra_solver.cpp
	g++ $(FLAGS) -c src/dijkstra_solver.cpp -o bin/dijkstra_solver.o

astar_solver: src/astar_solver.cpp
	g++ $(FLAGS) -c src/astar_solver.cpp -o bin/astar_solver.o

clean:
	rm -f bin/*.o data/outputs/*.txt
