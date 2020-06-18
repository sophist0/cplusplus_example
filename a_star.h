#include <vector>
#include <unordered_map>
#include <string>

using namespace std;

/** Declare A* functions */
void run_a_man(string data_path);

/** Declare General functions */
std::unordered_map<std::string,int> precom_man(State);

int get_manhattan(State c_state);

int get_fn(Node c_node, std::unordered_map<std::string,int> man_dict);

vector <Node> combine_sorted(vector <Node> small_vec, vector <Node> large_vec);

bool sort_node_cost(Node n1, Node n2);
