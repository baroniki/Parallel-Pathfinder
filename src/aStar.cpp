#pragma once

#include <iostream>

#include <ostream>

#include <fstream>

#include <vector>

#include <list>

#include <string>

#include <queue>

#include <unordered_map>

#include <algorithm>

#include <bits/stdc++.h>

#include <time.h>





using namespace std;



class MAPFInstance {

public:

	vector<int> start_locations;

	vector<int> goal_locations;

	int num_of_agents = 1;



	// return true if the location is blocked by an obstacle.

	inline bool blocked(int location) const {return my_map[location]; }

	inline size_t map_size() const { return rows * cols; }

 

	// This can be used as admissible heuristics

	int get_Manhattan_distance(int from, int to) const;

 

	list<int> get_adjacent_locations(int location) const; // return unblocked adjacent locations

 	bool load_instance(const string& fname); // load instance from file

	void print_instance() const;
    //JOSES
    void print_instance_path(vector<int> paths) const;

    int rows  = 34;

    int cols = 34;//34

private:

	vector<bool> my_map; // my_map[i] = true iff location i is blocked

////////////////////////////////////////////////////////////////////////////////

//Hard coding the values for the size of the maze here	

	

	enum valid_moves_t { NORTH, EAST, SOUTH, WEST, WAIT_MOVE, MOVE_COUNT };  // MOVE_COUNT is the     enum's size

	int moves_offset[5];

	

 	inline int linearize_coordinate(int row, int col) const { return (this->cols * row + col); }

	inline int row_coordinate(int location) const { return location / this->cols; }

	inline int col_coordinate(int location) const { return location % this->cols; }

};













bool MAPFInstance::load_instance(const string& fname) {

    ifstream myfile (fname.c_str(), ios_base::in);

    if (myfile.is_open()) {
  


        my_map.resize(rows * cols);	//change value based on size of map



        // read map

        char c;

	int start_x, start_y, goal_x, goal_y;

        for (int i = 0; i < rows; i++) {

            for (int j = 0; j < cols; j++) {

                myfile.get(c);

		if(c == 'S'){

			start_x = i;

			start_y = j;

			my_map[cols * i + j] = (c != 'S'); // the cell is unblocked iff it is 'S'		

		}

		else if (c == 'X'){

			goal_x = i;

			goal_y = j;

			my_map[cols * i + j] = (c != 'X'); // the cell is unblocked iff it is 'X'

		}

		else{

			my_map[cols * i + j] = (c != ' '); // the cell is unblocked iff it is ' '

		}

	    }

        }



        //myfile >> num_of_agents; // read the number of agents

        start_locations.resize(1);

        goal_locations.resize(1);



        // read the start and goal locations

      	// int start_x, start_y, goal_x, goal_y;

        //for (int i = 0; i < num_of_agents; i++) {

        //    myfile >> start_x >> start_y >> goal_x >> goal_y;

        start_locations[0] = linearize_coordinate(start_x, start_y);

        goal_locations[0] = linearize_coordinate(goal_x, goal_y);

        //}

        myfile.close();



        // initialize moves_offset array

        moves_offset[valid_moves_t::WAIT_MOVE] = 0;

        moves_offset[valid_moves_t::NORTH] = -cols;

        moves_offset[valid_moves_t::EAST] = 1;

        moves_offset[valid_moves_t::SOUTH] = cols;

        moves_offset[valid_moves_t::WEST] = -1;

        return true;

    } else

        return false;

}











void MAPFInstance::print_instance() const {

    cout << "Map:" << endl;

    for (int i = 0; i < rows; i++) {

        for (int j = 0; j < cols; j++) {

            // if (my_map[cols * i + j])

            //    cout << "@"; // obstacle

            // else


            //    cout << " "; // free cell
            

        }

       // cout << endl;

    }

    //cout << num_of_agents << " agents:" << endl;

    //for (int i = 0; i < num_of_agents; i++) {

    cout << start_locations[0] << "-->" << goal_locations[0] << endl;

    //}

}
void MAPFInstance:: print_instance_path( vector<int> paths ) const{
    // cout << "Final Map:" << endl;

    // for (int i = 0; i < rows; i++) {

    //     for (int j = 0; j < cols; j++) {
    //         cout << my_map[cols*i+j];

    //         if (my_map[cols * i + j])

    //             cout << "@"; // obstacle

    //         else{
    //             for (int k=0; k <paths.size(); k++){
                  
    //                 if(my_map[cols * i +j] == paths[k]){
    //                     cout << "X";
    //                 }
    //             }


    //             cout << " "; // free cell
    //         }

    //     }

    //     cout << endl;

    // }

}











int MAPFInstance::get_Manhattan_distance(int from, int to) const {

    int from_x = row_coordinate(from);

    int from_y = col_coordinate(from);

    int to_x = row_coordinate(to);

    int to_y = col_coordinate(to);

    return abs(from_x - to_x) + abs(from_y - to_y);

}



list<int> MAPFInstance::get_adjacent_locations(int location) const {

	list<int> locations;

	for (int direction : moves_offset) {

		int next_location = location + direction;

		if (0 <= next_location && next_location < cols * rows && // next_location is on the map

		    !my_map[next_location] && // next_location is not blocked

		    get_Manhattan_distance(location, next_location) <= 1) // it indeed moves to a neighbor location

            		locations.push_back(next_location);

	}

	return locations;

}







// Path is a sequence of locations,

// where path[i] represents the location at timestep i

typedef vector<int> Path;

ostream& operator<<(ostream& os, const Path& path); // used for printing paths



// A hash function used to hash a pair of any kind

// use pair as the key of an unordered_map

struct hash_pair {

    template <class T1, class T2>

    size_t operator()(const pair<T1, T2>& p) const

    {

        auto hash1 = hash<T1>{}(p.first);

        auto hash2 = hash<T2>{}(p.second);

        return hash1 ^ hash2;

    }

};















struct AStarNode {

    int location;

    int g;

    int h;

    AStarNode* parent;



    AStarNode(): location(-1), g(-1), h(-1), parent(nullptr) {}

    AStarNode(int location, int g, int h, AStarNode* parent):

            location(location), g(g), h(h), parent(parent) {}

};



// This function is used by priority_queue to prioritize nodes

struct CompareAStarNode {

    bool operator()(const AStarNode* n1, const AStarNode* n2) {

        if (n1->g + n1->h == n2->g + n2->h) // if both nodes have the same f value,

            return n1->h > n2->h; // break ties by preferring smaller h value

        else

            return n1->g + n1->h > n2->g + n2->h; // otherwise, prefer smaller f value

    }

};



class AStarPlanner {

public:

    const MAPFInstance& ins;



    AStarPlanner(const MAPFInstance& ins): ins(ins) {}

    Path find_path(int agent_id);

private:

    // used to retrieve the path from the goal node

    Path make_path(const AStarNode* goal_node) const;

    void clear();

};







ostream& operator<<(ostream& os, const Path& path)

{

    for (auto loc : path) {

        os << loc << " ";

    }

    return os;

}



Path AStarPlanner::make_path(const AStarNode* goal_node) const {

    Path path;

    const AStarNode* curr = goal_node;

    while (curr != nullptr) {

        path.push_back(curr->location);

        curr = curr->parent;

    }

    std::reverse(path.begin(),path.end());

    return path;

}



Path AStarPlanner::find_path(int agent_id) {

    struct timespec start, stop;

    double time;

    int start_location = ins.start_locations[agent_id];

    int goal_location = ins.goal_locations[agent_id];



    // Open list

    priority_queue<AStarNode*, vector<AStarNode*>, CompareAStarNode> open;



    // Unordered map is an associative container that contains key-value pairs with unique keys.

    // The following unordered map is used for duplicate detection, where the key is the location of the node.

    unordered_map<int, AStarNode*> all_nodes;



    int h = ins.get_Manhattan_distance(start_location, goal_location); // h value for the root node

    auto root = new AStarNode(start_location, 0, h, nullptr);

    open.push(root);



    Path path;

///////////////////////////////////////////////////////////////////////////////////////////////

//This is where the searching for the next node is happening



    if( clock_gettime( CLOCK_REALTIME, &start) == -1 ) { perror( "clock gettime" );}



    while (!open.empty()) {

        auto curr = open.top();

        open.pop();



        // goal test

        if (curr->location == goal_location){ //

            path = make_path(curr);

            break;

        }

	

	//This is where we can create the parrallelism is happening

		

        // generate child nodes

        for (auto next_location : ins.get_adjacent_locations(curr->location)) {

            auto it = all_nodes.find(next_location);

	    if (it == all_nodes.end()) {// the location has not been visited before

                int next_g = curr->g + 1;

                int next_h = ins.get_Manhattan_distance(next_location, goal_location);

		auto next = new AStarNode(next_location, next_g, next_h, curr);

                open.push(next);

                all_nodes[next_location] = next;

            }

            // Note that if the location has been visited before,

            // next_g + next_h must be greater than or equal to the f value of the existing node,

            // because we are searching on a 4-neighbor grid with uniform-cost edges.

            // So we don't need to update the existing node.

        }

    }

    

    if( clock_gettime( CLOCK_REALTIME, &stop) == -1 ) { perror( "clock gettime" );}

    time = (stop.tv_sec - start.tv_sec)+ (double)(stop.tv_nsec - start.tv_nsec)/1e9;



    printf("Execution time = %f sec\n", time);

//////////////////////////////////////////////////////////////////////////////////////////////





    // release memory

    for (auto n : all_nodes)

        delete n.second;



    return path;

}















int main(int argc, char *argv[]) {

    MAPFInstance ins;

    string input_file = argv[1];

    // string output_file = argv[2];


    if (ins.load_instance(input_file)) {

        ins.print_instance();

    } else {

        cout << "Fail to load the instance " << input_file << endl;

        exit(-1);

    }



    AStarPlanner a_star(ins);

    vector<Path> paths(ins.num_of_agents);

    for (int i = 0; i < ins.num_of_agents; i++) {

        paths[i] = a_star.find_path(i); // plan paths independently by ignoring other agents

    }



    // print paths

    cout << "Paths:" << endl;

    int sum = 0;

    for (int i = 0; i < ins.num_of_agents; i++) {

        cout << "a" << i << ": " << paths[i] << endl;

        sum += (int)paths[i].size() - 1;

    }

    cout << "Sum of cost: " << sum << endl;
    vector<int> path_locations;
    for (unsigned int i=0; i<paths.size(); i++){
        path_locations.push_back((int)paths[i].size()-1);
    }

        ins.print_instance_path(path_locations);



    // save paths

    // ofstream myfile (output_file.c_str(), ios_base::out);

    // if (myfile.is_open()) {

    //     for (int i = 0; i < ins.num_of_agents; i++) {

    //         myfile << paths[i] << endl;

    //     }

    //     myfile.close();

    // } else {

    //     cout << "Fail to save the paths to " << output_file << endl;

    //     exit(-1);

    // }



    return 0;

}