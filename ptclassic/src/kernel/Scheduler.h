#ifndef _Scheduler_h
#define _Scheduler_h 1

#include "type.h"
#include "Galaxy.h"

class Scheduler {
public:
	// The dummy expressions are just to prevent compiler warnings
	// about galaxy being unused.
	virtual int setup(Block& galaxy) {&galaxy;}
	virtual int run(Block& galaxy) {&galaxy;}
	virtual int wrapup(Block& galaxy) {&galaxy;}
};

class SDFScheduler : public Scheduler {
public:
	int setup(Block& galaxy);
private:
	// set the repetitions property of each atomic block
	// in the galaxy, and recursively call the repetitions
	// function for each non-atomic block in the galaxy.
	int repetitions (Galaxy& galaxy);

	// set the repetitions property of each atomic block is
	// the subgraph (which may be the whole graph) connected
	// to block.  Called by repetitions().
	int reptConnectedSubgraph(Block& block);

	// set the repetitions property of blocks on each side of
	// a connection.  Called by reptConnectedSubgraph().
	int reptArc(SDFPortHole& port1, SDFPortHole& port2);

	// least common multiple of the denominators of the repetitions
	// member of a connected subgraph
	int lcm;
};

/*******
Interface of scheduler to topology.
	class SpaceWalk
	repetitions
********/
#endif
