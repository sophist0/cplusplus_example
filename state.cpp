/** Contains Function Defs */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <limits>

#include "state.h"

using namespace std;

/** ################################################################### */
/** 			Gen. Functions					*/
/** ################################################################### */

/** Gets min if a vector of ints */
int min_int(vector <int> int_vec){
	int min = numeric_limits<int>::max();
	for (int i=0; i<int_vec.size(); i++){

		if (int_vec[i] < min){
			min = int_vec[i];
		}
	}

	return min;
}

/** Gets max if a vector of ints */
int max_int(vector <int> int_vec){
	int max = numeric_limits<int>::min();
	for (int i=0; i<int_vec.size(); i++){
		if (int_vec[i] > max){
			max = int_vec[i];
		}
	}
	return max;
}

vector <int> add_unique_int(vector <int> integer_vec, int el){

	/** integer_vec is passed by reference, its only passed back for clarity */
	bool in_vec = false;
	for (int i=0; i<integer_vec.size(); i++){
		if (integer_vec[i] == el){
			in_vec = true;
			break;
		}		
	}
	if (in_vec == false){
		integer_vec.push_back(el);
	}

	return integer_vec;	
}

vector <int> swapIdx(int idx1, int idx2, vector <int> state){	
	for(int i=0; i<state.size(); i++){
		if (state[i]==idx1){
			state[i]=idx2;
		}
		else if (state[i]==idx2){
			state[i]=idx1;
		}
	}
	return state;
}

/** Write output short form */
void write_output_short(vector <Node> path){

	cout << endl;
        int len = -1; // length of path at initial state is zero
        for (int j=path.size()-1; j >=0; j--){

                if (path[j-1].p_move.m_dir == 'l'){
                        cout << "(" << path[j-1].p_move.m_id << ", left)" << endl;
                }
                else if (path[j-1].p_move.m_dir == 'r'){
                        cout << "(" << path[j-1].p_move.m_id << ", right)" << endl;
                }
                else if (path[j-1].p_move.m_dir == 'u'){
                        cout << "(" << path[j-1].p_move.m_id << ", up)" << endl;
                }
                else if (path[j-1].p_move.m_dir == 'd'){
                        cout << "(" << path[j-1].p_move.m_id << ", down)" << endl;
                }
        }
}

/** Get path to solution node */
vector <Node> get_path(Node f_node, vector <Node> known_nodes){

        vector <Node> path;
        int node_id = f_node.n_id;
        int parent_id = -1; 
        path.push_back(f_node);

        while (node_id > 0){ 
                parent_id = f_node.p_id;

                // find parent node
                for (int i=0; i < known_nodes.size(); i++){
                        if (known_nodes[i].n_id == parent_id){
                                f_node = known_nodes[i];
                                break;
                        }
                }
                path.push_back(f_node);
                node_id = f_node.n_id;
        }
        return path;
}

/** ################################################################### */
/** 			Node Class Functions				*/
/** ################################################################### */

/** Constructor values initialized (ROOT)*/
Node::Node(State new_state, int root_id){

	node_count = root_id; // set node count
	n_id = root_id;
	p_id = -1;
	p_move = Move(-2,'x');
	n_depth = 0;
	n_cost = 0;
	n_state = new_state;
}

/** Constructor values initialized (NOT ROOT)*/
Node::Node(State new_state, int parent_id, int parent_depth, int parent_cost, Move move){

	node_count ++; // increase node count
	n_id = node_count;
	p_id = parent_id;
	p_move = move;
	n_depth = parent_depth + 1;
	n_cost = parent_cost + 1;
	n_state = new_state;
}

/** Constructor cloned nodes no values initialized, node count not increased */
Node::Node(){
}

Node Node::clone_node(){

	Node node_clone;
	node_clone.n_id = this->n_id;
	node_clone.p_id = this->p_id;
	node_clone.p_move = this->p_move;
	node_clone.n_depth = this->n_depth;
	node_clone.n_cost = this->n_cost;
	node_clone.n_state = this->n_state;

	return node_clone;
}

vector <Node> Node::expand(std::unordered_map<std::string,int> known_states){

	vector <Node> child_nodes;
	vector <Move> state_moves;
	state_moves = this->n_state.get_all_moves();

	/** Get new states and compare to old states */	
	for (int i=0; i<state_moves.size(); i++){
		State tmp_state1;
		tmp_state1 = this->n_state.apply_move_cloning(state_moves[i]);

		/** norm tmp_state2 while preserving unnormed tmp_state1. Assumes all past states have already been normed !!! */
		State tmp_state2 = tmp_state1.clone_state();
		tmp_state2.norm_state();
		string sid = tmp_state2.state_id();

		if (known_states[sid] != 1){
			Node c_node(tmp_state1, this->n_id, this->n_depth, this->n_cost, state_moves[i]);
			child_nodes.push_back(c_node);
		}
	}

	return child_nodes;
}


/** ################################################################### */
/** 			State Class Functions				*/
/** ################################################################### */

/** Constructor no values initialized */
State::State(){
}

/** Print current state */
void State::print_state(){

        cout << endl << "row: " << this->p_rows << ", " << "col: " << this->p_cols << endl << endl;
        for (int i=0; i<this->p_rows; i++){
                for (int j=0; j<this->p_cols; j++){
                        cout << this->p_state[i*this->p_cols+j] << ",";
                }
                cout << endl;
        }
}

/** Load new state */
void State::load_state(string fname){

	/** Pass in filename and cols, rows by reference */
        ifstream infile;
        infile.open(fname.c_str());	
        if(infile.fail()){
                cout << "Error opening " << fname << endl;
        }

        /** Get array parameters */
        string f_str;
        getline(infile,f_str,',');
        int cols = stoi(f_str);
        getline(infile,f_str,',');
        int rows = stoi(f_str);
      
        /** state vector */
        vector<int> state_vec(cols*rows);
        for (int i=0; i<rows; i++){
                for (int j=0; j<cols; j++){

                        getline(infile,f_str,',');
                        state_vec[i*cols+j]=stoi(f_str);
                }
        }    
        infile.close();

	/** Set public variables */
	this->p_cols = cols;
	this->p_rows = rows;
	this->p_state = state_vec;
}

string State::state_id(){
	// Assumes state is normed

	vector<int> state_vec = this->p_state;
	string state_id;
	for (int i=0; i<state_vec.size(); i++){
		if ((state_vec[i] != 1) && (state_vec[i] != -1)){
			state_id += to_string(state_vec[i]);
		}
	}
	return state_id;
}

/** Clone state */
State State::clone_state(){

	State state_clone;
	state_clone.p_cols = this->p_cols;
	state_clone.p_rows = this->p_rows;
	state_clone.p_state = this->p_state;
	return state_clone;
}

/** Solved check */
bool State::check_solved(){

	bool solved = true;
	vector <int> state = this->p_state;

	for(int i=0; i<state.size(); i++){
		if (state[i] == -1){
			solved = false;
			break;
		}
	}
	return solved;
}

/** Gets pieces moves */
vector <Move> State::get_moves(int id){

	/** get all rows and cols of piece */
	/** note currently row and col values are not unique in all_cols all_rows */
	vector <int> state = this->p_state;
	int cols = this->p_cols;
	int rows = this->p_rows;
	vector <char> id_moves;
	vector <Move> moves;
	vector <int> all_cols;
	vector <int> all_rows;

	bool valid_id = false;
	for (int i=0; i<rows; i++){
                for (int j=0; j<cols; j++){

               		if (state[i*cols+j] == id){
				all_cols.push_back(j);
				all_rows.push_back(i);
				valid_id = true;
			}
                }
        }   
	if (valid_id == false){
		cout << endl << "Piece does not exist" << endl;
		return moves;
	}

	/** get min/max col/row of piece */
	int min_col = min_int(all_cols);
 	int max_col = max_int(all_cols);
	int min_row = min_int(all_rows);
	int max_row = max_int(all_rows);

	/** can move to 0 or -1 spaces */

	/** check if left move is valid */
	bool left = true;
	for (int i=0; i<all_rows.size(); i++){
		
		if (min_col-1 < 0){
			left = false;
		}
		else if ((id != 2) && (state[all_rows[i]*cols+min_col-1] !=0)){
			left = false;
		}
		else if ((id == 2) && (state[all_rows[i]*cols+min_col-1] !=0) && (state[all_rows[i]*cols+min_col-1] !=-1)){
			left = false;
		}
	}
	if (left == true){
		id_moves.push_back('l');
	}

	/** check if right move is valid */
	bool right = true;
	for (int i=0; i<all_rows.size(); i++){

		if (max_col+1 > cols-1){
			right = false;
		}
		else if ((id != 2) && (state[all_rows[i]*cols+max_col+1] !=0)){
			right = false;
		}
		else if ((id == 2) && (state[all_rows[i]*cols+max_col+1] !=0) && (state[all_rows[i]*cols+max_col+1] !=-1)){
			right = false;
		}
	}
	if (right == true){
		id_moves.push_back('r');
	}

	/** check if up move is valid */
	bool up = true;
	for (int i=0; i<all_cols.size(); i++){

		if (min_row-1 < 0){
			up = false;
		}
		else if ((id != 2) && (state[(min_row-1)*cols + all_cols[i]] !=0)){
			up = false;
		}
		else if ((id == 2) && (state[(min_row-1)*cols + all_cols[i]] !=0) && (state[(min_row-1)*cols + all_cols[i]] !=-1)){
			up = false;
		}
	}
	if (up == true){
		id_moves.push_back('u');
	}

	/** check if down move is valid */
	bool down = true;
	for (int i=0; i<all_cols.size(); i++){

		if (max_row+1 > rows-1){
			down = false;
		}
		else if ((id != 2) && (state[(max_row+1)*cols + all_cols[i]] !=0)){
			down = false;
		}
		else if ((id == 2) && (state[(max_row+1)*cols + all_cols[i]] !=0) && (state[(max_row+1)*cols + all_cols[i]] !=-1)){
			down = false;
		}
	}
	if (down == true){
		id_moves.push_back('d');
	}

	for (int i=0; i<id_moves.size(); i++){
		Move tmp_move(id,id_moves[i]);
		moves.push_back(tmp_move);
	} 

	return moves;
}

/** Gets all moves */
vector <Move> State::get_all_moves(){

	vector <Move> all_moves;
	vector <int> state = this->p_state;
	vector <int> all_ids;

	/** get all pieces in state */
	for (int i=0; i<state.size(); i++){
		if (state[i] > 1){
			all_ids = add_unique_int(all_ids, state[i]);
		}
	}

	/** get all moves for pieces */
	for (int i=0; i<all_ids.size(); i++){
		vector <Move> tmp = get_moves(all_ids[i]);
				
		/** insert tmp into all moves */
		all_moves.insert(all_moves.end(), tmp.begin(), tmp.end());
	}

	return all_moves;
}

/** Apply move and update state */
void State::apply_move(Move new_move){

	int id = new_move.m_id;
	char dir = new_move.m_dir;
	int cols = this->p_cols;
	vector <int> state = this->p_state;

	/** get pieces current locations */
	vector <int> c_loc;
	for (int i=0; i<state.size(); i++){
		if (state[i] == id){
			c_loc.push_back(i);
			/** null out piece */
			state[i] = 0;
		}
	}
	/** move left -> subtract 1 from c_loc*/
	if (dir == 'l'){
		for (int i=0; i<c_loc.size(); i++){
			state[c_loc[i]-1] = id;
		}
	}
	/** move right -> add 1 to c_loc*/
	else if (dir == 'r'){
		for (int i=0; i<c_loc.size(); i++){
			state[c_loc[i]+1] = id;
		}
	}
	/** move left -> subtract number of cols from c_loc*/
	else if (dir == 'u'){
		for (int i=0; i<c_loc.size(); i++){
			state[c_loc[i]-cols] = id;
		}
	}
	/** move down -> add number of cols to c_loc*/
	else{
		for (int i=0; i<c_loc.size(); i++){
			state[c_loc[i]+cols] = id;
		}
	}

	/** Save new state */
	this->p_state = state;
}

/** Apply move Cloning */
State State::apply_move_cloning(Move new_move){
	
	State state_clone = clone_state();
	state_clone.apply_move(new_move);

	return state_clone;
}

/** Compare it states are identical */
bool State::compare_states(State cmp_state){

	vector <int> cmp_state_vec = cmp_state.p_state;
	vector <int> state_vec = this->p_state;
	bool identical = true;

	/** assumes both states are the same size */
	for (int i=0; i<state_vec.size(); i++){
		if (state_vec[i] != cmp_state_vec[i]){
			identical = false;
			break;
		}
	}

	return identical;	
}

/** Put current state in normal form */
void State::norm_state(){

	/** using idea provided in assignment modded for state vector */
	vector <int> state_vec = this->p_state;
	int nextIdx = 3;

	for (int i=0; i<state_vec.size(); i++){
		if (state_vec[i] == nextIdx){
			nextIdx++;
		}
		else if (state_vec[i] > nextIdx){
			state_vec = swapIdx(nextIdx, state_vec[i], state_vec);
			nextIdx++;
		}
	}
	/** Save normalized State */
	this->p_state = state_vec;	
}

/** ################################################################### */
/** 			Move Class Functions				*/
/** ################################################################### */

/** Constructor no values initialized */
Move::Move(){
}

/** Constructor values initialized */
Move::Move(int piece_id, char move_dir){
	m_id = piece_id;
	m_dir = move_dir;
}

void Move::print_move(){

	if (this->m_dir == 'l'){
		cout << "(" << this->m_id << ", left)" << endl;
	}
	else if (this->m_dir == 'r'){
		cout << "(" << this->m_id << ", right)" << endl;
	}
	else if (this->m_dir == 'u'){
		cout << "(" << this->m_id << ", up)" << endl;
	}
	else if (this->m_dir == 'd'){
		cout << "(" << this->m_id << ", down)" << endl;
	}
}

/** ################################################################### */
/** 			Location Class / Struct				*/
/** ################################################################### */

/** Constructor values initialized */
Loc::Loc(int row, int col){
	l_row = row;
	l_col = col;
}

/** Print locations */
void Loc::print_loc(){
	cout << endl << "(" << this->l_row << "," << this->l_col << ")" << endl;
}

