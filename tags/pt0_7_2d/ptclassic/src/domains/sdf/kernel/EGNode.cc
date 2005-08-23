static const char file_id[] = "EGNode.cc";

/******************************************************************
Version identification:
@(#)EGNode.cc	1.15	3/5/96

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code. '
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif
 
#include "EGNode.h"

////////////////////
// EGNode methods //
////////////////////

EGNode :: EGNode(DataFlowStar* s, int n) : invocation(n), pStar(s), next(0) {
	stickyFlag = FALSE;
	if (n == 1) s->setMaster(this);
}

EGNode :: ~EGNode() {}

void EGNode :: deleteInvocChain() {
	if (next) {
		next->deleteInvocChain();
		LOG_DEL; delete next;
		next = 0;
	}
}

StringList EGNode :: printMe() {
	StringList out;
	out << printShort() << "-- ANCESTORS --\n" << ancestors.printMe()
	    << "\n -- DESCENDANTS --\n" << descendants.printMe();
	return out;
}

StringList EGNode :: printShort() {
	StringList out = "Star: name = ";
	if (pStar) out << pStar->name();
	else out << "noName";
	out << " (invocation # " << invocation << ")\n";
	return out;
}

StringList EGNodeList :: print() {
	StringList out = "\n<Node lists>\n";

	EGNodeListIter nlist(*this);
	EGNode* n;
	while ((n = nlist++) != 0)
		out << n->printShort();
	return out;
}
	
EGNode* EGNode::getInvocation(int i) {

	if (i < invocation) return 0;
	else if (i == invocation) return this;
	else if (next) return next->getInvocation(i);
	else return 0;
}

EGGate* EGNode::makeArc(EGNode *dest, int samples, int delay) 
{
#if ARCTRACE
	printf("Making arc from ");
	print();
	printf(" to ");
	dest->print();
	printf(" (nsam=%d,delay=%d).\n", samples, delay);
#endif

	LOG_NEW; EGGate* destnode = new EGGate(dest);
	LOG_NEW; EGGate* srcnode = new EGGate(this);
	srcnode->allocateArc(destnode, samples, delay);

	// Insert the srcnode
	if (descendants.insertGate(srcnode, 1)) {
		dest->ancestors.insertGate(destnode,0);
		return srcnode;
	}

	delete srcnode;
	delete destnode;
	return 0;
}

// Determine whether this node is a root of the expanded
// graph. A node is a root if it either has no ancestors,
// or if each arc in the ancestor list has enough delay on it.
int EGNode::root() {
	EGGateLinkIter nextAncestor(ancestors);
	EGGate *p;
	while((p = nextAncestor++) != 0) {
		if ((p->delay() < p->samples()) || (p->samples() == 0))
			return FALSE;
	}
	return TRUE;    
}
