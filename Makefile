FLAGS = -g -Wall -std=c++11 -fopenmp

all: run

run: maze_solver bfs_solver src/main.cpp
	g++ $(FLAGS) src/main.cpp bin/maze_solver.o bin/bfs_solver.o -o run

maze_solver: src/maze_solver.cpp
	g++ $(FLAGS) -c src/maze_solver.cpp -o bin/maze_solver.o

bfs_solver: src/bfs_solver.cpp
	g++ $(FLAGS) -c src/bfs_solver.cpp -o bin/bfs_solver.o

clean:
	rm -f bin/*.o data/outputs/*.txt
