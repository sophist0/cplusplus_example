#include <iostream>

#include "state.h"
#include "a_star.h"

using namespace std;

/** Init. static var */
int Node::node_count = 0;

int main(){

	/** Run Manhattan SBP-level1 */

        string fname = "data/SBP-level1.txt";
	run_a_man(fname);

	/////////////////////////////////////////

	/** Run Manhattan SBP-level2 */

        fname = "data/SBP-level2.txt";
        //fname = "data/SBP-bricks-level4.txt";

	run_a_man(fname);

      	return 0;
}
