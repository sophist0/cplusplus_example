##################################
Run and Compile
##################################

To compile:

	make clean
	make

(make clean will cause an error if there are no *.o files. If this occurs ignore it.)

To run:

	./puzzle_sol

##################################
Block Puzzle
##################################

Given an (n,m) block puzzle with a board width n and height m. On the board are numbered spaces, the exit is -1, free spaces are 0's, fixed edges are 1's, the master block 2's, all other blocks are numbered 2 or greater. Blocks can be of various sizes, for instance

3	(1x1 block)

3,3	(1x2 block)

3	(2x1 block)
3

etc.

The goal of the puzzle is to find the fewest moves of all blocks numbered 2 and greater such that the master block reaches the exit.

##################################
Example: SBP-level1.txt
##################################

Initial Puzzle State:

 1,1,1,1,1
 1,3,2,2,1
 1,0,4,5,1
-1,0,6,7,1
 1,1,1,1,1

Simplified representation

  3,2,2
  0,4,5
  0,6,7

1: 6 left; 7 left; 4 left

  3,2,2
  4,0,5
  6,7,0

2: 5 down; 2 down

  3,0,0
  4,2,2
  6,7,5

3: 3 right

  0,3,0
  4,2,2
  6,7,5

4: 4 up

  4,3,0
  0,2,2
  6,7,5

5: 2 left

  4,3,0
  2,2,0
  6,7,5

6: 5 up

  4,3,0
  2,2,5
  6,7,0

7: 7 right

  4,3,0
  2,2,5
  6,0,7

8: 5 up; 7 up

  4,3,5
  2,2,7
  6,0,0

9: 6 right; 6 right

  4,3,5
  2,2,7
  0,0,6

10: 2 down

  4,3,5
  0,0,7
  2,2,6

10: 2 left

  4,3,5
  0,0,7
2,2,0,6

Final Puzzle State:

 1,1,1,1,1
 1,4,3,5,1
 1,0,0,7,1
 2,2,0,6,1
 1,1,1,1,1

This solution was found with A-star using the Manhattan distance between the master block and the exit as the heuristic.

###########################################
A-star Algorithm and this implementation
###########################################

A-star maybe implemented on a tree or graph comprised of nodes and edges. The block puzzle is a graph search problem since it contains possible cycles. For example in given the initial state above the following set of moves creates a cycle.

state: 0

  3,2,2
  0,4,5
  0,6,7

state: 1; 4 left

  3,2,2
  4,0,5
  0,6,7

state: 2; 4 right

  3,2,2
  0,4,5
  0,6,7

In my implementation each puzzle state is contained in an instance of the Node class. The Node class in full contains the following

Node Class

	Variables

		node id
		parent node id
		parent move producing current node
		node depth (number of nodes required to reach node)
		node cost (f(n) estimated cost to get to the goal from the root node via node n. Confusingly this is initialized to something else and then updated by A-Star)
		puzzle state

	Functions

		clone_node -> clones current node
		expand children -> expands all child nodes which have not previously been expanded (this requires checking against previously expanded nodes)

Most of the variables in Node class are required for recovering the path to the solution rather than finding it.

The idea of A-star is given a starting position or root node in a search tree or a search graph instead of expanding all children as in BFS or blindly choosing a branch and exploring it fully as in DFS, A-star estimates from which child node (or more accurately which edge node of the currently explored graph) it will cost the least to reach the goal node. In this case the goal node is a node where the master block has reached the exit. A-star's estimated cost for a node n can be written as

f(n) = g(n) + h(n)

where g(n) is the cost of getting to node n and h(n) is the estimated cost of getting from node n to the goal node. In our case the estimated cost of getting from node n to the goal node is the Manhattan distance from the position of the master block in puzzle state of node n to the position of the exit.

For example say we are at node m0

  3,2,2
  4,0,0
x,5,6,7

where x is the exit space and the 2's represent the master block. m0 has 4 children

m1:

  3,0,0
  4,2,2
x,5,6,7

m2:

  3,2,2
  0,4,0
x,5,6,7

m3:

  3,2,2
  4,6,0
x,5,0,7

m4:

  3,2,2
  4,0,7
x,5,6,0

Lets assume the cost of getting to m0 is 2 such that g(m0)=2, it follows that 

g(m1) = g(m2) = g(m3) = g(m4) = g(m0) + 1 = 3

If h(n) is the Manhattan distance from the location of the master block to the exit in node n, then

h(m1) = 3

while

h(m2) = h(m3) = h(m4) = 4 

and therefore

f(m1) = g(m1) + h(m1) = 6

while

f(m2) = f(m3) = f(m4) = 7 

therefore A-star would explore the children of node m1 before those of m2, m3, or m4. However if node m1's children all have estimated costs greater then 7, A-star will backtrack to explore the children of nodes m2, m3, and m4 before continuing to explore the branch containing node m1.

It's clear from this example that A-star can be loosely thought of as a form of greedy search where the searches greed is directed by the chosen heuristic. In fact if the heuristic is well chosen A-star provides guarantees generally not available to greedy search algorithms. For instance a heuristic h(n) is consistent if for every node n and any successor n' which can be arrived at via the search graph

h(n) <= c(n,n') + h(n')

where c(n,n') is the cost of going from node n to node n'. If h(n) is consistent it can be show that h(n) is also admissible, meaning that it never over estimates to cost of getting to the goal node and additionally it can be shown that A-star's solution is optimal, yielding the lowest cost path to the goal node.

One trick I employ in this algorithms implementation is to notice that if the puzzle state of node n is,

  3,2,2
  0,4,0
  5,6,7

and the puzzle state of successor node m is

  3,2,2
  0,4,0
  5,6,7

then

h(n) <= c(n,m) + h(m)

and node m's children need not be explored since they are identical to node n's children. Additionally if q is a successor node of n and the puzzle state of q is

  3,2,2
  0,6,0
  5,4,7

node q is identical to the puzzle state of node n up to the arbitrary numbering of blocks and A-star can safely ignore node q if it has already discovered node n. Also notice that this pruning of puzzle states that are identical up to block numbering effectively transforms the A-stars search graph into a search tree.

#####################################################################################
Reference: Artificial Intelligence A Modern Approach - Stuart Russell, Peter Norvig 
#####################################################################################
