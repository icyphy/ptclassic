static const char file_id[] = "EGNode.cc";

/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code. '
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif
 
#include "EGNode.h"

////////////////////
// EGNode methods //
////////////////////

EGNode :: EGNode(SDFStar* s, int n) : pStar(s), invocation(n), next(0) {
	if (n == 1) s->setMaster(this);
}

EGNode :: ~EGNode() {}

void EGNode :: deleteInvocChain() {
	if (next) next->deleteInvocChain();
	LOG_DEL; delete this;
}

StringList EGNode :: printMe() {
	StringList out;
	out += printShort();
	out += "-- ANCESTORS --\n";
	out += ancestors.printMe();
	out += "\n -- DESCENDANTS --\n";
	out += descendants.printMe();
	return out;
}

StringList EGNode :: printShort() {
	StringList out;
	out += "Star: name = ";
	if (pStar)
		out += pStar->readName();
	else
		out += "noName";
	out += " (invocation # ";
	out += invocation;
	out += ")\n";
	return out;
}

StringList EGNodeList :: print() {
	StringList out;
	out += "\n<Node lists>\n";

	EGNodeListIter nlist(*this);
	EGNode* n;
	while ((n = nlist++) != 0)
		out += n->printShort();
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
	printf(" (nsam=%d,delay=%d).\n",samples,delay);
#endif

	LOG_NEW; EGGate *destnode = new EGGate(dest);
	LOG_NEW; EGGate *srcnode = new EGGate(this);
	srcnode->allocateArc(destnode, samples, delay);
	descendants.insertGate(srcnode,1);
	dest->ancestors.insertGate(destnode,0);
	return srcnode;
}

// Determine whether this node is a root of the expanded
// graph. A node is a root if it either has no ancestors,
// or if each arc in the ancestor list has enough delay on it.
int EGNode::root() {
	EGGateLinkIter nextAncestor(ancestors);
	EGGate *p;
	while((p=nextAncestor++)!=0) {
		if ((p->delay() < p->samples()) || (p->samples() == 0))
			return FALSE;
	}
	return TRUE;    
}

