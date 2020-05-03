#ifndef MAZE_TREE_H
#define MAZE_TREE_H

#include <vector>
#include <map>

class MazeTree {
public:
	MazeTree(std::string maze_file);
	vector<Node*> nodes;
};

class Node {
private:
	//Type 1 maze variables
	std::vector<int> adjacencies;
	int level;
	Node* pred;
	bool path;

public:
	//Type 1 Maze Functions
	Node();
	std::vector<int> getAdj();
	void addAdj(int index);

	int getLevel();
	void setLevel(int level);

	void setPred(Node* pred);
	Node* getPred();
	void setPath();
	bool onPath();
};


#endif
