static const char file_id[] = "ExpandedGraph.cc";

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ExpandedGraph.h"
#include "GalIter.h"
#include "SDFConnect.h"
#include "Geodesic.h"
#include "EGConnect.h"

//////////////////////
// Utility routines //
//////////////////////

// Check whether a state which is modified during runtime,
// exists in a block.
int StateExists(SDFStar& star)
{
  	BlockStateIter nextState(star);
  	State* s;

  	while ((s=nextState++)!=0) {
    		if ((s->attributes() | AB_CONST) == 0) 
      		return(1);
  	}
  	return(0);
}  // StateExists

// This function checks whether the argument star uses past input or outputs. 

int PastPortsUsed(SDFStar& star) 
{
	BlockPortIter nextPort(star);
	SDFPortHole* p;

	while ((p = (SDFPortHole*) nextPort++) != 0) {
		if (p->usesOldValues())
			return TRUE;
	}
	return FALSE;
}

///////////////////////////
// ExpandedGraph methods //
///////////////////////////

ExpandedGraph :: ~ExpandedGraph() {
	EGMasterIter nextMaster(*this);
	EGNode* m;

	while ((m = nextMaster++) != 0) {
		m->deleteInvocChain();
	}
}

void ExpandedGraph::initialize() {}

EGGate* ExpandedGraph::connect_invocations(SDFStar* src, int i, SDFStar* dest, 
                                      int j, int n_sam, int n_d)
{

#if EGDEBUG
	printf("    -> connect invocations : %s{%d} to %s{%d}; %d %d\n",
		src->readName(),i,dest->readName(),j, n_sam, n_d);
#endif

	EGNode* source = src->myMaster()->getInvocation(i);
	EGNode* destination = dest->myMaster()->getInvocation(j);
	if ((!source) || (!destination)) return FALSE;
	return makeArc(source, destination, n_sam, n_d);
}

// Identify which port and which sample is assigned to each gate.
// This information is necessary for parallel code generation.
void ExpandedGraph :: setGate(EGGate* src_gate, PortHole* src_port, int src_ix,
	     PortHole* dest_port, int dest_ix) {

	// for the source gate
	src_gate->setProperty(src_port,src_ix);
	
	// for the destination gate
	src_gate->farGate()->setProperty(dest_port,dest_ix);
}

// This function creates a expanded graph node for each invocation of 
// each star in the argument galaxy.
void ExpandedGraph::initialize_invocations()
{
  	GalStarIter nextStar(*myGal);
  	SDFStar *s;

  	while ((s = (SDFStar*)nextStar++) != 0) {
		nodecount += int(s->repetitions);
		createInvocations(s);
	}
}

// This function creates all invocations of a star.
void ExpandedGraph::createInvocations(SDFStar* s)
{
	int num = s->repetitions;
	EGNode* prev = 0;
	for (int i = num; i > 0 ; i--) {
		EGNode * new_node = newNode(s,i);

		// Make a linked list of all instances of each star.
		new_node->setNextInvoc(prev);
		prev = new_node;
	}

	// Put the first instance of each star into the master list.
	masters.append(prev);
}

//
// This function creates the dataflow links correspoding to the arc 
// whose information is passed in the arguments :
// p = number of samples produced by source
// q = number of samples consumed by dest
// d = number of delays on the arc
// src = source star
// dest = dest star
//
// It is assumed that the nodes for each invocation of each star have been
// created already, and the invocation pointer for each star initialized.
// 
// Return value : 1 if no errors found
//                0 if there were problems
 
int ExpandedGraph::ExpandArc(SDFStar* src, PortHole* src_port, 
			     SDFStar* dest, PortHole* dest_port)
{
	// set up local variables for connection information.
	int p = src_port->numberTokens;
	int q = dest_port->numberTokens;
	int d = dest_port->myGeodesic->numInit();

	int src_invocation = 1;
	int dest_invocation;
	int src_sample = 1;
  
	int Vdest = dest->repetitions;
	int Vsrc = src->repetitions;
	int total_samples = Vdest*q;

	// calculate to which invocation and sample of the
	// destination, the current source node will go
	int dest_sample = 1+((d%total_samples)%q);
	dest_invocation = 1+ ((d%total_samples)/q);
	int hom_delay = d/total_samples;

	int test, dummy;
	EGGate* sGate;
	do { 

		// calculate if the source node or the dest node has
		// to switch samples first
		test = (p - src_sample) - (q - dest_sample);

		if (test == 0) {
			// both src and dest must switch invocations together
			dummy = p - src_sample + 1;
			if(!(sGate = connect_invocations(src, src_invocation, 
                		dest, dest_invocation, dummy, hom_delay)))
				return FALSE;  

			// set the property of the gates just created.
			setGate(sGate, src_port, src_sample, dest_port,
				dest_sample);

			src_sample = 1; 
			dest_sample = 1;
			src_invocation++;
			dest_invocation++;
		}
		else if (test < 0) {
			// src node switches invocations first
			dummy = p - src_sample + 1;
			if(!(sGate = connect_invocations(src, src_invocation, 
                		dest, dest_invocation, dummy, hom_delay)))
				return FALSE;  

			// set the property of the gates just created.
			setGate(sGate, src_port, src_sample, dest_port,
				dest_sample);

			src_sample = 1; 
			dest_sample += dummy; 
			src_invocation++;
		}
		else {
			// dest node switches invocations first
			dummy = q - dest_sample + 1;
			if(!(sGate = connect_invocations(src, src_invocation, 
                		dest, dest_invocation, dummy, hom_delay)))
				return FALSE;  

			// set the property of the gates just created.
			setGate(sGate, src_port, src_sample, dest_port,
				dest_sample);

			dest_sample = 1; 
			src_sample += dummy; 
			dest_invocation++;
		}

		// make sure current invocation of A stays in proper range
		if (src_invocation > Vsrc) src_invocation=1;
		if (dest_invocation > Vdest) {
			dest_invocation=1;
			hom_delay++;
		}

		total_samples -= dummy;
		if (total_samples < 0) {  
			StringList msg = "While creating expanded graph, sample rate problem detected between\n";
			msg += src->readFullName();
			msg += " and ";
			msg += dest->readFullName();
			Error::abortRun(msg);
			return FALSE;
		}
	} while (total_samples > 0);    
	return TRUE;
}

// This function checks whether a star depends on previous invocations 
// of itself. If it does, then an arc is inseted between each invocation
// and its successor, and an arc with delay 1, is inserted from the last 
// invocation to the first.

int ExpandedGraph::SelfLoop(SDFStar& s)
{ 
	if ( StateExists(s) || PastPortsUsed(s)) {

		// connect successive invocations of the star as a chain of 
		// precedences with an arc of delay 1 inserted from the 
		//last invocation to the first.

		int repNum = s.repetitions;
		for (int i = 1; i< repNum; i++) 
			if (!connect_invocations(&s,i,&s,i+1,0,0)) 
				return FALSE;
    
		if (repNum > 1) 
			return int(connect_invocations(&s,repNum,&s,1,0,1)); 
    		return(TRUE); 
  	}
	return(TRUE);
} // SelfLoop    



////  constructor for creating the expanded graph from a galaxy /////

int ExpandedGraph::createMe(Galaxy& galaxy)
{
	SDFStar *dest, *source;
	SDFPortHole *dest_port, *source_port;
	GalStarIter nextStar(galaxy);

	myGal = &galaxy;

	initialize(); 

	// invoke all instances (EGNodes) of the blocks
	initialize_invocations();

	// make connections among EGNodes
	while ((dest = (SDFStar*)nextStar++)!=0) {

	   // make connections between all instances of the same Star if
	   // current invocation depends on the previous invocation.
	   if (!SelfLoop(*dest)) return FALSE;

	   BlockPortIter nextPort(*dest);
	   while((dest_port = (SDFPortHole*)nextPort++)!=0) {
		source_port = (SDFPortHole*)dest_port->far();
      		if (dest_port->isItInput() && source_port->isItOutput()) {
			source = (SDFStar*)source_port->parent();

			// main routine to connect EGNodes
        		if (!ExpandArc(source,source_port,dest,dest_port)) 
				return FALSE;
		}
	   }
	}

	// find the root nodes in the graph, and count up
	// the number of nodes, to pass to the reachability-matrix-
	// builder.

	EGIter nextNode(*this);
	EGNode *p;
	while ((p=nextNode++)!=0) {
		if (p->root()) insertSource(p);
	}
	return TRUE;
}

StringList ExpandedGraph::display() {
	EGIter nextNode(*this);
	EGNode *p;
	StringList out;

	out += " ** Expanded Graph ** \n";
	while ((p=nextNode++)!=0) {
		out += p->printMe(); 
	}
	out += "\n";
	return out;
}

void ExpandedGraph::removeArcsWithDelay()
{
	EGIter nextNode(*this);
	EGNode *p;
	EGGate *q;

	while ((p=nextNode++)!=0) {
		EGGateLinkIter preciter(p->ancestors);
		while ((q=preciter++) != 0) 
			if (q->delay() > 0) q->removeMyArc(); 
	}
}

EGNode *ExpandedGraph::newNode(SDFStar *s, int i) {
	LOG_NEW; EGNode *tmp = new EGNode(s,i);
	return tmp;
}

//////////////////////
// EGIter methods   //
//////////////////////

// return the next node in the traversal sequence
EGNode* EGIter::next() 
{
	EGNode* temp = ref_node;
	if (ref_node) {
		ref_node = ref_node->getNextInvoc();
		if (!ref_node) {
			ref_node = (*next_master)++;
		}
	}
	return temp;
}

