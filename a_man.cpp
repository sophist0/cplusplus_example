#include <iostream>
#include <vector>
#include <algorithm>    // random shuffle
#include <ctime>        // std::time
#include <cstdlib>      // std::rand, std::srand
#include <time.h>	// for clock objects
#include <cmath>
#include <unordered_map>
#include <string>

#include "state.h"
#include "a_star.h"

using namespace std;

void run_a_man(string fname){

	// Start timer
	clock_t t1,t2;
	t1 = clock();

	/** Creates product object */
	State root_state;

	/** Create state vectors */
	vector <Node> closed_nodes;
	vector <Node> open_nodes;
	vector <Node> all_nodes;
	std::unordered_map<std::string, int> all_node_states;

	/** Load state */
        int cols, rows;
	vector <int> state_vec;

	cout << endl << fname.substr(5,fname.size()-5) << " manhattan" << endl;
	cout << endl << "Initial state";
	root_state.load_state(fname);

	/** Given root state compute all Manhattan distance */
	std::unordered_map<std::string,int> man_dict = precom_man(root_state);

	/** Print state */
        root_state.print_state();

	/*******************************/
	/* A-Star Manhattan	       */ 
	/*******************************/

	/** get root node */
	Node n_root(root_state,0);

	/** push root onto open list */
	int tmp = get_fn(n_root, man_dict);
	n_root.n_cost = tmp; // Set cost equal to fn
	open_nodes.push_back(n_root);
	all_nodes.push_back(n_root);
	string sid = n_root.n_state.state_id();
	all_node_states.insert({sid,1});

	bool solved = false;
	int count = 0;
	int idx; // index of node to expand from open nodes
	while (solved == false){

		count++;
		// Find the node in the closed list with the min f(n) = h(n) + g(n)
		// find the fn each sub block of the master block and return min 

		// Notice that this selects the last node with index i such that fn <= min_fn, 
		// there could be more then one but A-star by definition does not necessarily explore all moves with min_fn although A-star can also backtrack.

		int fn;
		int min_fn = 1000000; // Assume max fn < 1 million
		for(int i=0; i<open_nodes.size(); i++){
			fn  = open_nodes[i].n_cost;
			if (fn <= min_fn){
				min_fn = fn;
				idx = i;
			}
		}

		// check if node to be expanded is a solution state
		solved = open_nodes[idx].n_state.check_solved();

		if (solved == false){

			// expand node with min fn
			vector <Node> expanded_nodes;
			expanded_nodes = open_nodes[idx].expand(all_node_states);

			// add expanded node to closed list
			closed_nodes.push_back(open_nodes[idx]);
			open_nodes.erase(open_nodes.begin()+idx);

			// add expanded nodes to open list
			for (int i=0; i<expanded_nodes.size(); i++){
				int fn_tmp = get_fn(expanded_nodes[i], man_dict);
				// Set cost equal to fn
				expanded_nodes[i].n_cost = fn_tmp; 
				open_nodes.push_back(expanded_nodes[i]);

				// save normed nodes to history
				Node clone_node = expanded_nodes[i].clone_node();
				clone_node.n_state.norm_state();
				all_nodes.push_back(clone_node);
				string sid = clone_node.n_state.state_id();
				all_node_states.insert({sid,1});
			}

		}
		if ((closed_nodes.size() % 100) == 0){
			cout << endl << "nodes explored: " << closed_nodes.size();
		}

	} 
	cout << endl;
	t2 = clock();

	// Get path to this solution state
	vector <Node> path;
	path = get_path(open_nodes[idx],all_nodes);

	// print solution (Assumes files are in subdir data/)
	write_output_short(path);
	cout << endl << "Final state";
	open_nodes[idx].n_state.print_state();

	// print stats; number of nodes explored is the number of nodes in the closed list +1 for the final state; length is path.size()-1 not counting the initial state
	cout << endl << "nodes: " << closed_nodes.size() <<", time: " << ((float)t2 - (float)t1) / CLOCKS_PER_SEC << " [s]" << ",length: " << path.size()-1;
	cout << endl << endl;
	cout << "##########################################################" << endl << endl;
}

// Sorts nodes via there costs
bool sort_node_cost(Node n1, Node n2){
	bool sorted = false;
	if (n1.n_cost <= n2.n_cost){
		sorted = true;
	}
	return sorted;
}

int get_fn(Node c_node, std::unordered_map<std::string, int> man_dict){

	int depth = c_node.n_depth;
	State c_state = c_node.n_state;

	int rows = c_state.p_rows;
	int cols = c_state.p_cols;
	vector<int> state_vec = c_state.p_state;
	int m_dst = 0;

	/** Find upper left corner of the master block */
        for (int i=0; i<rows; i++){
                for (int j=0; j<cols; j++){
			if (state_vec[i*cols+j] == 2){
				// upper left corner of the master block
				string key = to_string(i) + to_string(j);
				m_dst = man_dict[key];
				break;
			}
                }
        }
	return m_dst + depth;
}

std::unordered_map<std::string,int> precom_man(State state){
	/** Pre-compute the Manhattan distances */

	State c_state = state.clone_state();
	int rows = c_state.p_rows;
	int cols = c_state.p_cols;
	vector <int> state_vec = c_state.p_state;
	vector <int> m_rows;
	vector <int> m_cols;
	std::unordered_map<std::string,int> man_dict;

	/** Wipe all blocks greater then 1 and get master block width and height */	
        for (int i=0; i<rows; i++){
                for (int j=0; j<cols; j++){
			if (state_vec[i*cols+j] == 2){
				m_rows = add_unique_int(m_rows,i);
				m_cols = add_unique_int(m_cols,j);
			}
			if (state_vec[i*cols+j] > 1){
				state_vec[i*cols+j] = 0;
			}
                }
        }

	/** Calculate Manhattan distance for all possible position, (i,j) are the upper left corner of the master block */
	for (int i=1; i<(rows-m_rows.size()); i++){
		for (int j=1; j<(cols-m_cols.size()); j++){
			
			// Add master block
			for (int k=i; k < i+m_rows.size(); k++){		
				for (int l=j; l < j+m_cols.size(); l++){			
					state_vec[k*cols+l] = 2;
				}
			}

			// Get Manhattan distance
			c_state.p_state = state_vec;
			int dst = get_manhattan(c_state);
			string key = to_string(i) + to_string(j);
			man_dict.insert({key,dst});		// key is the concatenation of i and j

			// Delete master block
			for (int k=i; k < i+m_rows.size(); k++){		
				for (int l=j; l < j+m_cols.size(); l++){			
					state_vec[k*cols+l] = 0;
				}
			}
		}
	}
	return man_dict;
}


// Pre-compute for each location to save time?
int get_manhattan(State c_state){

	int rows = c_state.p_rows;
	int cols = c_state.p_cols;
	vector <int> state_vec = c_state.p_state;

	// locations of the master block (i,j), (rows, cols)
	vector <Loc> master_loc;

	// locations of the exit (i,j), (rows, cols)
	vector <Loc> exit_loc;
	
        for (int i=0; i<rows; i++){
                for (int j=0; j<cols; j++){
			if (state_vec[i*cols+j] == 2){
				Loc m_loc(i,j);
				master_loc.push_back(m_loc);
			}
			if (state_vec[i*cols+j] == -1){
				Loc e_loc(i,j);
				exit_loc.push_back(e_loc);
			}
                }
        }

	// find min. manhattan distance to the exit point
	int min_dst = rows*cols;
	if (exit_loc.size() == 0){ // if at a solution state
		min_dst = 0;
	}		
	else{

		bool row_goal = false;
		bool col_goal = false;

		bool g_below = false;
		bool g_above = false;
		bool g_left = false;
		bool g_right = false;

		// goal in row or col? Assuming the goal state is not in the corners, state is a rectangle, etc.
		if ((exit_loc[0].l_row == 0) or (exit_loc[0].l_row == rows-1)){
			row_goal = true;
		}
		else{
			col_goal = true;
		}

		for (int x=0; x < master_loc.size(); x++){
			for (int y=0; y < exit_loc.size(); y++){

				// check goal below master block	
				if ((row_goal == true) and (master_loc[x].l_row > exit_loc[y].l_row)){
					g_above = true;
				}

				// check goal above master block
				if ((row_goal == true) and (master_loc[x].l_row < exit_loc[y].l_row)){
					g_below = true;
				}

				// check goal left of master block
				if ((col_goal == true) and (master_loc[x].l_col > exit_loc[y].l_col)){
					g_left = true;
				}

				// check goal right of master block
				if ((col_goal == true) and (master_loc[x].l_col < exit_loc[y].l_col)){
					g_right = true;
				}
			}
		}

		// Get points to calculate the Manhattan distance from master block to goal.
		// Which part of the master block is chosen for this calculation depends on the location of the master block relative to the goal.
		Loc m_point(-1,-1);
		Loc e_point(-1,-1);
		if (g_above == true){
			//cout << endl << "ABOVE!" << endl;
			m_point.l_row = rows;
			m_point.l_col = cols;
			e_point.l_row = rows;
			e_point.l_col = cols;

			// top left corner -> (min,min)
			for (int x=0; x < master_loc.size(); x++){
				if (master_loc[x].l_row < m_point.l_row){
					m_point.l_row = master_loc[x].l_row;
				}
				if (master_loc[x].l_col < m_point.l_col){
					m_point.l_col = master_loc[x].l_col;
				}
			}
			for (int x=0; x < exit_loc.size(); x++){
				if (exit_loc[x].l_row < e_point.l_row){
					e_point.l_row = exit_loc[x].l_row;
				}
				if (exit_loc[x].l_col < e_point.l_col){
					e_point.l_col = exit_loc[x].l_col;
				}
			}
		}

		if (g_below == true){
			//cout << endl << "BELOW!" << endl;
			m_point.l_row = -1;
			m_point.l_col = -1;
			e_point.l_row = -1;
			e_point.l_col = -1;

			// bottom left corner -> (max,max)
			for (int x=0; x < master_loc.size(); x++){
				if (master_loc[x].l_row > m_point.l_row){
					m_point.l_row = master_loc[x].l_row;
				}
				if (master_loc[x].l_col > m_point.l_col){
					m_point.l_col = master_loc[x].l_col;
				}
			}
			for (int x=0; x < exit_loc.size(); x++){
				if (exit_loc[x].l_row > e_point.l_row){
					e_point.l_row = exit_loc[x].l_row;
				}
				if (exit_loc[x].l_col > e_point.l_col){
					e_point.l_col = exit_loc[x].l_col;
				}
			}
		} 

		if (g_left == true){
			//cout << endl << "LEFT!" << endl;
			m_point.l_row = -1;
			m_point.l_col = cols;
			e_point.l_row = -1;
			e_point.l_col = cols;

			// bottom right corner -> (max,min)
			for (int x=0; x < master_loc.size(); x++){
				if (master_loc[x].l_row > m_point.l_row){
					m_point.l_row = master_loc[x].l_row;
				}
				if (master_loc[x].l_col < m_point.l_col){
					m_point.l_col = master_loc[x].l_col;
				}
			}
			for (int x=0; x < exit_loc.size(); x++){
				if (exit_loc[x].l_row > e_point.l_row){
					e_point.l_row = exit_loc[x].l_row;
				}
				if (exit_loc[x].l_col < e_point.l_col){
					e_point.l_col = exit_loc[x].l_col;
				}
			}
		} 

		if (g_right == true){
			//cout << endl << "RIGHT!" << endl;
			m_point.l_row = rows;
			m_point.l_col = -1;
			e_point.l_row = rows;
			e_point.l_col = -1;

			// top left corner -> (min,max)
			for (int x=0; x < master_loc.size(); x++){
				if (master_loc[x].l_row < m_point.l_row){
					m_point.l_row = master_loc[x].l_row;
				}
				if (master_loc[x].l_col > m_point.l_col){
					m_point.l_col = master_loc[x].l_col;
				}
			}
			for (int x=0; x < exit_loc.size(); x++){
				if (exit_loc[x].l_row < e_point.l_row){
					e_point.l_row = exit_loc[x].l_row;
				}
				if (exit_loc[x].l_col > e_point.l_col){
					e_point.l_col = exit_loc[x].l_col;
				}
			}
		}

		for (int i=0; i<master_loc.size(); i++){

			int dst = abs(e_point.l_row - m_point.l_row) + abs(e_point.l_col - m_point.l_col);
			if (dst < min_dst){
				min_dst = dst;
			}
		}
	}
	return min_dst;
}
