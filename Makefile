FLAGS = -std=c++11

######################################################

puzzle_sol: puzzle_sol.o a_man.o state.o
	$(CXX) $(FLAGS) puzzle_sol.o a_man.o state.o -o puzzle_sol


hw2.o: puzzle_sol.cpp state.h
	$(CXX) $(FLAGS) -c puzzle_sol.cpp 

######################################################

a_man.o: a_man.cpp state.h a_star.h
	$(CXX) $(FLAGS) -c a_man.cpp 

######################################################

state.o: state.cpp state.h
	$(CXX) $(FLAGS) -c state.cpp

all: puzzle_sol

clean:
	-rm *.o
