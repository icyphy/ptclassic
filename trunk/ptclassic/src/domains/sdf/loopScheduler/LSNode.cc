static const char file_id[] = "LSNode.cc";
/******************************************************************
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

 Programmer:  Soonhoi Ha based on Shuvra Bhattacharyya's work
 Date of creation: 4/92

 LSNode

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "LSNode.h"
#include "EGGate.h"

int LSNode::fireable() {
	EGGateLinkIter nextParent(ancestors);
	EGGate* d;
	if (alreadyFired()) return FALSE;
	while ((d = nextParent++) != 0) {
		LSNode* temp = (LSNode*) d->farEndNode();
    		if (!(temp->alreadyFired())) return FALSE;
	}
	return TRUE;
}

// return the LSNode which is connected to this node and has the
// highest invocation index among the nodes of the same master.

LSNode* LSNode :: nextConnection(LSNode* model, int flag) {
	EGGateLinkIter iter(descendants);
	if (!flag) iter.reconnect(ancestors);
	EGGate* p;
	EGGate* prev = 0;
	DataFlowStar* m = model->myMaster();

	while ((p = iter++) != 0) {
		if (p->farEndMaster() == m) {
			prev = p;
			while ((p = iter++) != 0) {
				if (p->farEndMaster() != m) {
					return (LSNode*) prev->farEndNode();
				}
				prev = p;
			}
			return (LSNode*) prev->farEndNode();
		}
	}
	return 0;
}
	
// update outside connection from this node to another node not included
// in the argument cluster node.
void LSNode :: updateOutsideConnections(LSNode* cnode) {

	// With my ancestors
	EGGateLinkIter nextGate(ancestors);
	EGGate* g;

	while ((g = nextGate++) != 0) {
		LSNode* outNode = (LSNode*) g->farEndNode();
		if (!outNode->inActiveCluster()) {
			int samples = g->samples();

			// remove this intercluster arc
			LOG_DEL; delete g;

			// insert a link to this outside node
			outNode->makeArc(cnode,samples,0);
		}
	}

	// With my descendants
	nextGate.reconnect(descendants);

	while ((g = nextGate++) != 0) {
		LSNode* outNode = (LSNode*) g->farEndNode();
		if (!outNode->inActiveCluster()) {
			int samples = g->samples();
			LOG_DEL; delete g;
			cnode->makeArc(outNode,samples,0);
		}
	}
}
