static const char file_id[] = "ParNode.cc";

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

Programmer: Soonhoi Ha
Date of last revision:

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ParNode.h"
#include "ParGraph.h"

                        ///////////////////////
                        ///  *Constructor*  ///
                        ///////////////////////

ParNode::ParNode(DataFlowStar* s, int invoc_no) : EGNode(s, invoc_no)
{
	StaticLevel = 0;	// Initialize to something invalid
	type = 0;		
	procId = 0;
	exTime = s->myExecTime();
	waitNum = 0;
	origin = 0;
	partner = 0;
}

// Alternate constructor for idle nodes and communication nodes
// If type = 1, it is an idle node.
// If type = -1, it is a send node, type = -2 indicates a receive node

ParNode::ParNode(int t) : EGNode(0,0) {
	
	type = t;
	StaticLevel = 0;	// Initialize to something invalid
	procId = 0;
	exTime = 0;
	waitNum = 0;
	origin = 0;
	partner = 0;
}

                           ///////////////
                           ///  print  ///
                           ///////////////
// Prints the star name, level, execution time.

StringList ParNode::print() {

	StringList out;
	out += printShort();
	out += " (level = ";
	out += StaticLevel;
	out += ", execution time = ";
	out += myMaster()->myExecTime();
	out += ")\n";
	return out;
}

                        /////////////////////
                        ///  copyAncDesc  ///
                        /////////////////////
// This function copies the ancestors and descendants of the node
//	into temporary variables tempAncs and tempDescs.
// If flag = 0, copy normally
// If flag = 1, copy ancestors into tempDescs and descendants into tempAncs

void ParNode::copyAncDesc(ParGraph* g, int flag) {

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

// set informations for sub-universe generation
void ParNode :: setCopyStar(DataFlowStar* s, ParNode* prevN) {
	clonedStar = s;
	if (prevN) {
		prevN->nextNode = this;
		firstNode = prevN->firstNode;
		firstNode->numCopied++;
	} else {
		firstNode = this;
		numCopied = 1;
	}
	nextNode = 0;
}

