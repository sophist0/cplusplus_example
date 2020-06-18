/* Defines classes */

#include <vector>
#include <unordered_map>

using namespace std;

/** ########################################################################## */

class Loc{

	/** Defines public functions and variables */
	public:
		int l_row;
		int l_col;

		/** Calls a constructor and sets initial values */
		Loc(int row, int col);

		/** Print location */
		void print_loc();

	/** Defines private functions and variables */
	private:
};


/** ########################################################################## */

class Move{

	/** Defines public functions and variables */
	public:
		int m_id;
		char m_dir;

		/** Calls a constructor which currently does nothing */
		Move();

		/** Calls a constructor and sets initial values */
		Move(int id, char dir);

		/** Calls a constructor and sets initial values */
		void print_move();

	/** Defines private functions and variables */
	private:
};

/** ########################################################################## */

class State{

	/** Defines public functions and variables */
	public:
		int p_cols;
		int p_rows;
		vector<int> p_state;

		/** Calls a constructor which currently does nothing */
		State();

		/** Loads state saved as CSV file */
		void load_state(string fname);

		/** Unique ID for the state */
		string state_id();
		
		/** Prints current state */
		void print_state();

		/** Clone current state */
		State clone_state();

		/** Check solved */
		bool check_solved();

		/** Get valid piece moves */
		vector <Move> get_moves(int id);

		/** Get all valid moves */
		vector <Move> get_all_moves();

		/** Apply move to current state */
		void apply_move(Move new_move);

		/** Apply move to current state */
		State apply_move_cloning(Move new_move);

		/** Compare if states are identical without normalization */
		bool compare_states(State cmp_state);

		/** Put current state in normal form */
		void norm_state();

	/** Defines private functions and variables */
	private:
};

/** ########################################################################## */

class Node{

	/** Defines public functions and variables */
	public:
		static int node_count; // total nodes in tree
		int n_id; // node id
		int p_id; // node parent id

		Move p_move; // parent move
		
		int n_depth; // node depth
		int n_cost; // node cost
		State n_state; // node state

		// node  children implicitly defined by n_state and all previous moves

		/** Def a constructor and sets initial values (ROOT)*/
		Node(State new_state, int root_id);
		
		/** Def a constructor and sets initial values (NOT ROOT)*/
		Node(State new_state, int parent_id, int parent_depth, int parent_cost, Move l_move);

		/** Def a constructor for cloned nodes */
		Node();

		/** Clone node */
		Node clone_node();

		/** Expand node children */
		vector <Node> expand(std::unordered_map<std::string, int> known_nodes);

	/** Defines private functions and variables */
	private:
};

/** ########################################################################## */

/** Declare General functions */
vector <Node> get_path(Node f_node, vector <Node> known_nodes);

vector <int> add_unique_int(vector <int> integer_vec, int el);

void write_output_short(vector <Node> path);
