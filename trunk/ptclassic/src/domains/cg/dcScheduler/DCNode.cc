static const char file_id[] = "DCNode.cc";
/*****************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

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
							COPYRIGHTENDKEY

Programmer: Soonhoi Ha based on G.C. Sih's code
Date of last revision: 5/92 

*****************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "DCNode.h"
#include "DCGraph.h"
#include "DCCluster.h"
#include "Error.h"

                        ///////////////////////
                        ///  *Constructor*  ///
                        ///////////////////////

DCNode::DCNode(DataFlowStar* s, int invoc_no) : ParNode(s, invoc_no)
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
