/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code.
 
*******************************************************************/

#ifndef _EGGate_h
#define _EGGate_h
#ifdef __GNUG__
#pragma interface
#endif

#include "DoubleLink.h"
#include "StringList.h"
#include "PortHole.h"

class EGGateLink;
class EGNode;
class DataFlowStar;

///////////////////////////////////////////////////////////////
//
// EGGate.h
//
// Dataflow is maintained by Gate's and Arc's. Each ExpandedGraph 
// node has a list of ancestors and descendants -- these are lists 
// of EGGate's, which point to Arc's. This is entirely analogous to
// the PortHole/Geodesic setup, but with much fewer overheads.
//
///////////////////////////////////////////////////////////////


/////////////////
// class EGArc //
/////////////////

// An arc between invocations (EGNode's) in the expanded
// graph. Arcs are referenced by EGGate's, which
// reside in ancestor and descendant lists of  EGNode's.

class EGArc {
public :
	// Constructor with the number of samples and delay
	// to be placed on the arc.
	EGArc(int s, int d) : arc_samples(s), arc_delay(d) {}
 
	// Get the number of samples on the arc. 
	int samples() { return arc_samples; }

	// Get the delay on the arc.
	int delay() { return arc_delay; }

	// Add x samples to the arc.
	void addSamples(int x) { arc_samples+=x; }

private :
	// The number of samples passed along the arc.
	int arc_samples;

	// The delay on the arc.
	int arc_delay;
};

////////////////////////
// class EGGate       //
////////////////////////
//
// This class is used to store a single entry in the list of ancestors or
// descendants for a node in the expanded graph. 
//

class EGGate {
public:
	EGGate(EGNode* n, PortHole* p = 0) : 
		parent(n), pPort(p), arc(0), far(0) {}
 
	virtual ~EGGate();

	// input or output?
	int isItInput() { return pPort->isItInput(); }

	// readName
	const char* name() const { return pPort->name(); }

	// hide me from the node
	void hideMe(int flag = 0);

	//  a pointer to the far end node
	EGNode* farEndNode() 
		{ if (far==0) return 0;
		  else return far->parent; }
	
	// far gate
	EGGate* farGate() { return far; }

	// set the original porthole and the index.
	void setProperty(PortHole* p, int i) 
		{ pPort = p; index = i; }

	// return the aliased porthole
	const PortHole* aliasedPort() { return pPort; }

	// print the information associated with this arc
	StringList printMe();

	// the master on the far end of this arc
	DataFlowStar *farEndMaster();

	// set link pointer
	void setLink(EGGateLink* p) { myLink = p; }
	EGGateLink* getLink() { return myLink; }

	// the invocation number of the far end node
	int farEndInvocation();

	// return the number of samples for this arc
	int samples()
		{ if (arc==0) return 0;
		  else return arc->samples(); }

	// return the delay along this arc
	int delay()
		{ if (arc==0) return 0;
		  else return arc->delay(); }

	void addSamples(int x) { arc->addSamples(x); } 

	// allocate & set up an arc between this gate and "dest"
	void allocateArc(EGGate *dest,int no_samples,int no_delays);

private:
	// the EG node for which this is a gate.
	EGNode *parent;

	// the arc which connects this gate to a gate in another node
	EGArc *arc;

	// a pointer to the far end gate, accross the arc
	EGGate *far;

	// pointer to my link
	EGGateLink* myLink;

	// pointer to the original porthole aliased from.
	PortHole* pPort;

	// Since a PortHole in the original block may need multiple
	// Gates in the corresponding EGNode, we maintain the index
	// of each Gate, which represents the order of the Gates 
	// associated with the same PortHole
	int index;
};   

class EGGateList;

//////////////////////////
// class EGGateLink     //
//////////////////////////

class EGGateLink : public DoubleLink
{
friend class EGGateList;
public:
	EGGate* gate() { return (EGGate*) e; }
	EGGateLink* nextLink() { return (EGGateLink*) next; }

	EGGateLink(EGGate* e) : DoubleLink(e), myList(0) {}

	void removeMeFromList();

private:
	EGGateList* myList;
};

//////////////////////////
// class EGGateList     //
//////////////////////////
// 
// This class contains a list of EGGateLink entries -- either the 
// list of ancestors or the list of descendants for a node in the expanded 
// graph.
//
// The following ordering is maintained in precedence lists : entries for the
// same EGNode occur together (one after another), and they occur in
// order of increasing invocation number. Entries for the same invocation 
// occur in increasing order of the number of delays on the arc.
//

class EGGateList : public DoubleLinkList
{
public:
	DoubleLink* createLink(EGGate* e)
		{ INC_LOG_NEW; EGGateLink* tmp = new EGGateLink(e);
		  e->setLink(tmp);
		  return tmp; }
	
	// Insert a new gate into the proper position in the precedence list. 
	// The update parameter indicates whether or not to
	// update the arc data if a node with the same "farEndNode",
	// and delay, already exits. This is required
	// so the arc wont get updated twice, for the insertion
	// of each endpoint. If update is 0, the entire arc for
	// "p" will be deallocated -- since this arc ("p") is redundant.
	// (If update is 1, we will still try to update the other
	// end of "p" later, so don't deallocate now.)
	//
	void insertGate(EGGate *p, int update);

	void initialize();

	// destructor :
	// this makes sure that "arc"'s get deallocated too
	~EGGateList();

	// print out the list
	StringList printMe();

private:
	// Search the list for the first entry pointing to "master"   
	EGGate* findMaster(DataFlowStar *master);

	// Search the list for the point where "node" should be inserted
	// into the list.
	EGGateLink* findInsertPosition(EGNode *n, int delay, int& ret);
};

/////////////////////////////////////
// class EGGateLinkIter            //
/////////////////////////////////////
//
// iterator class for EGGateList
//

class EGGateLinkIter : public DoubleLinkIter {
public:
	EGGateLinkIter(const EGGateList& l) : 
		DoubleLinkIter(l), refMaster(0) {}
	EGGate *next() {return (EGGate*) DoubleLinkIter::next();}
 
	EGGate *operator++ () {return next();}
	void reset() { DoubleLinkIter :: reset(); refMaster = 0; }

	// return the next gate connected to a new master that is not
	// the same as the argument master.
	EGGate* nextMaster(DataFlowStar*);

private:
	DataFlowStar* refMaster;
};

#endif

