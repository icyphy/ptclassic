static const char file_id[] = "DCNode.cc";
/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha based on G.C. Sih's code
Date of last revision: 5/92 

*****************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DCNode.h"
#include "DCGraph.h"
#include "Cluster.h"
#include "Error.h"

                        ///////////////////////
                        ///  *Constructor*  ///
                        ///////////////////////

DCNode::DCNode(SDFStar* s, int invoc_no) : ParNode(s, invoc_no)
{
	scheduledTime = -1;	// Initialize to something invalid
	finishTime = -1;	// Initialize to something invalid

	TClosure.initialize();
	RTClosure.initialize();
}

// Alternate constructor for communication nodes
// If type = -1, it is a send node, type = -2 indicates a receive node
DCNode::DCNode(int type) : ParNode(type) {
	StaticLevel = 0;	// Initialize to something invalid
	scheduledTime = -1;	// Initialize to something invalid
	finishTime = -1;	// Initialize to something invalid
}

                        /////////////////////
                        ///  copyAncDesc  ///
                        /////////////////////
// This function copies the ancestors and descendants of the node
//	into temporary variables tempAncs and tempDescs.
// If flag = 0, copy normally
// If flag = 1, copy ancestors into tempDescs and descendants into tempAncs

void DCNode::copyAncDesc(DCGraph* g, int flag) {

	resetWaitNum();

	EGGateLinkIter dfiter1(ancestors);
	EGGateLinkIter dfiter2(descendants);
	EGGate *dfl;

	// Clear tempAncs and tempDescs completely
	tempAncs.initialize();
	tempDescs.initialize();

	// Restore tempAncs (sorted smallest SL first)
	while ((dfl = dfiter1++) != 0) {
	   if (flag == 0)
		g->sortedInsert(tempAncs, (ParNode*)(dfl->farEndNode()), 0);
	   else
		g->sortedInsert(tempDescs, (ParNode*)(dfl->farEndNode()), 0);
	}

	// Restore tempDescs (sorted largest SL first)
	while ((dfl = dfiter2++) != 0) {
	   if (flag == 0)
		g->sortedInsert(tempDescs, (ParNode*)(dfl->farEndNode()), 1);
	   else
		g->sortedInsert(tempAncs, (ParNode*)(dfl->farEndNode()), 1);
	}
}

                        ////////////////////
                        ///  getSamples  ///
                        ////////////////////
// Returns the number of samples passed from *this to destnode
int DCNode::getSamples(DCNode *destnode)
{
	EGGateLinkIter giter(destnode->ancestors);
	EGGate *g;

	// Iterate through the ancestors of destnode
	while ((g = giter++) != 0) {
		if (g->farEndNode() == this)
			return g->samples();
	}
        // If here, can't find #samples between this and destnode
        Error::abortRun("Can't find samples in DCNode::getSamples");
	return 0;
}

                        //////////////////////
                        ///  adjacentNode  ///
                        //////////////////////

// return an adjacent node in the given node list.
// if direction = 1, look at the ancestors, if -1, descendants.

DCNode* DCNode :: adjacentNode(DCNodeList& nlist, int direction) {

	DCNodeListIter iter(tempAncs);
	DCNode* n;
	if (direction < 0) iter.reconnect(tempDescs);
	while ((n = iter++) != 0) 
		if (nlist.member(n)) return n;
	return 0;
}
	
                           ///////////////
                           ///  print  ///
                           ///////////////
// Prints the star name and invocation number of the node

StringList DCNode::print() {

	StringList out;
	DCNode *node;

	if (getType() < 0) { // Communication node
		if (getType() == -1) {
			out += "Send[";
			out += " - ";
			out += "]\n";
		}
		else {
			out += "Recv[";
			out += " - ";
			out += "]\n";
		}
	} else {	
		out += EGNode :: printShort();
	}
	
	return out;
}

// member
int DCNodeList :: member(DCNode* n) {
	DCNodeListIter iter(*this);
	DCNode* t;
	while ((t = iter++) != 0)
		if (t == n) return TRUE;
	return FALSE;
}

void DCNodeList :: removeNodes() {
	DCNode* n;
	while ((n = takeFromFront()) != 0) {
		LOG_DEL; delete n;
	}
	initialize();
}
