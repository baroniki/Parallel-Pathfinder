#include "node.h"
#include <iterator>
#include <fstream>


MazeTree::MazeTree(std::string maze_file) {
	
}


Node::Node(){
	level = -1;
	pred = NULL;
	path = false;
}

std::vector<int> Node::getAdj(){
	return adjacencies;
}

void Node::addAdj(int index){
	adjacencies.push_back(index);
}

int Node::getLevel(){
	return level;
}

void Node::setLevel(int level){
	this->level = level;
}

void Node::setPred(Node* pred){
	this->pred = pred;
}
Node* Node::getPred(){
	return pred;
}

void Node::setPath() {
	path = true;
}

bool Node::onPath() {
	return path;
}
