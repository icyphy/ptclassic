static const char file_id[] = "LSNode.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha based on Shuvra Bhattacharyya's work
 Date of creation: 4/92

 LSNode

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "LSNode.h"
#include "EGConnect.h"

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
	SDFStar* m = model->myMaster();

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
			g->removeMyArc();

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
			g->removeMyArc();
			cnode->makeArc(outNode,samples,0);
		}
	}
}
