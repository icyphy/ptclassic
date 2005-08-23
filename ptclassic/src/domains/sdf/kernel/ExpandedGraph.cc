static const char file_id[] = "ExpandedGraph.cc";

/******************************************************************
Version identification:
@(#)ExpandedGraph.cc	1.24	05/21/96

Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ExpandedGraph.h"
#include "GalIter.h"
#include "SDFPortHole.h"
#include "Geodesic.h"
#include "EGGate.h"

//////////////////////
// Utility routines //
//////////////////////

// This function checks whether the argument star uses past input or outputs. 

int PastPortsUsed(DataFlowStar& star) 
{
	DFStarPortIter nextPort(star);
	DFPortHole* p;
	while ((p = nextPort++) != 0) {
		if (p->usesOldValues()) return TRUE;
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
		LOG_DEL; delete m;
	}
}

void ExpandedGraph::initialize() {}

EGGate* ExpandedGraph::connect_invocations(DataFlowStar* src, int i,
					   DataFlowStar* dest, 
					   int j, int n_sam, int n_d)
{

#if EGDEBUG
	printf("    -> connect invocations : %s{%d} to %s{%d}; %d %d\n",
		src->readName(),i,dest->readName(),j, n_sam, n_d*n_sam);
#endif

	EGNode* source = src->myMaster()->getInvocation(i);
	EGNode* destination = dest->myMaster()->getInvocation(j);
	if ((!source) || (!destination)) return FALSE;
	return makeArc(source, destination, n_sam, n_d*n_sam);
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
  	DataFlowStar *s;
  	while ((s = (DataFlowStar*)nextStar++) != 0) {
		nodecount += int(s->repetitions);
		createInvocations(s);
	}
}

// This function creates all invocations of a star.
// The first invocation should be created first since the derived
// class use this fact for efficiency reason (e.g. ParNode constructor).
void ExpandedGraph::createInvocations(DataFlowStar* s)
{
	int num = s->repetitions;
	EGNode* prev = 0;
	for (int i = 1; i <= num ; i++) {
		EGNode* new_node = newNode(s,i);

		// Make a linked list of all instances of each star.
		if (prev) prev->setNextInvoc(new_node);
		prev = new_node;
	}

	// Put the first instance of each star into the master list.
	masters.append(s->myMaster());
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
 
int ExpandedGraph::ExpandArc(DataFlowStar* src, PortHole* src_port, 
			     DataFlowStar* dest, PortHole* dest_port)
{
	// set up local variables for connection information.
	int p = src_port->numXfer();
	int q = dest_port->numXfer();
	int d = dest_port->geo()->numInit();

	int Vsrc = src->repetitions;
	int Vdest = dest->repetitions;
	int total_samples = Vdest*q;

	int src_invocation = 1;
	int src_sample = 1;
  
	// calculate to which invocation and sample of the
	// destination, the current source node will go
	int dest_sample = 1 + ((d%total_samples)%q);
	int dest_invocation = 1 + ((d%total_samples)/q);
	int hom_delay = d/total_samples;

	do { 
		EGGate* sGate = 0;
		int dummy = 0;

		// calculate if the source node or the dest node has
		// to switch samples first
		int test = (p - src_sample) - (q - dest_sample);

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
			msg += src->fullName();
			msg += " and ";
			msg += dest->fullName();
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

int ExpandedGraph::SelfLoop(DataFlowStar& s)
{ 
	if ( enforcedSelfLoop || s.hasInternalState() ||
	     PastPortsUsed(s) || s.isItWormhole()) {
		parallelizable = FALSE;

		// connect successive invocations of the star as a
		// chain of precedences.
		EGNode* en = s.myMaster();
		en->claimSticky();
		int repNum = s.repetitions;
		for (int i = 1; i< repNum; i++) {
			en = en->getNextInvoc();
			en->claimSticky();
			if (!connect_invocations(&s,i,&s,i+1,0,0)) 
				return FALSE;
		}
    
    		return TRUE; 
  	}
	return TRUE;
} // SelfLoop    



////  constructor for creating the expanded graph from a galaxy /////

int ExpandedGraph::createMe(Galaxy& galaxy, int selfLoopFlag)
{
	GalStarIter nextStar(galaxy);
	DataFlowStar* dest;

	myGal = &galaxy;
	enforcedSelfLoop = selfLoopFlag;

	initialize(); 

	// invoke all instances (EGNodes) of the blocks
	initialize_invocations();

	// make connections among EGNodes
	parallelizable = TRUE;
	while ((dest = (DataFlowStar*)nextStar++) != 0) {

	   // make connections between all instances of the same Star if
	   // current invocation depends on the previous invocation.
	   if (!SelfLoop(*dest)) return FALSE;

	   DFStarPortIter nextPort(*dest);
	   DFPortHole* dest_port;
	   while((dest_port = nextPort++) != 0) {
		DFPortHole* source_port = (DFPortHole*)dest_port->far();
		if (!source_port) continue;
      		if (dest_port->isItInput() && source_port->isItOutput()) {
			DataFlowStar* source =
				(DataFlowStar*)source_port->parent();

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
	EGNode* p;
	while ((p = nextNode++) != 0) {
		if (p->root()) insertSource(p);
	}
	return TRUE;
}

StringList ExpandedGraph::display() {
	EGIter nextNode(*this);
	EGNode* p;
	StringList out = " ** Expanded Graph ** \n";
	while ((p = nextNode++) != 0) {
		out += p->printMe(); 
	}
	out += "\n";
	return out;
}

void ExpandedGraph::removeArcsWithDelay()
{
	EGIter nextNode(*this);
	EGNode *p;
	while ((p = nextNode++) != 0) {
		EGGateLinkIter preciter(p->ancestors);
		EGGate *q;
		while ((q = preciter++) != 0) 
			if (q->delay() > 0) 
				q->hideMe(); 
	}
}

EGNode* ExpandedGraph::newNode(DataFlowStar *s, int i) {
	LOG_NEW; EGNode* tmp = new EGNode(s,i);
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
