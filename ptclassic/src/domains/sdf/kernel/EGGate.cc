static const char file_id[] = "EGGate.cc";

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

#include "EGGate.h"
#include "EGNode.h"
#include "SDFStar.h"

StringList EGGate::printMe() {
	StringList out;
	out += "nsam = ";
	out += samples();
	out += ", and delay = ";
	out += delay();
	out += "\n";
	return out;
}

void EGGate::allocateArc(EGGate *dest, int no_samples, int no_delays) 
{
	far=dest;
	dest->far=this;
	LOG_NEW; arc = new EGArc(no_samples, no_delays);
	dest->arc = arc; 
}

// disconnect this node & it's far end node, and deallocate
// them.
EGGate::~EGGate() 
{
	myLink->removeMeFromList();
	if (far) { 
		far->far = 0;
		LOG_DEL; delete arc; 
		LOG_DEL; delete far;
		far = 0;
	}
}

void EGGate::hideMe(int flag) {
	if (!flag) far->hideMe(1);
	myLink->removeMeFromList();
	parent->hiddenGates.insertGate(this, 0);
}
	
DataFlowStar* EGGate :: farEndMaster() {
	if (far == 0) return 0;
	return farEndNode()->myMaster(); }

int EGGate :: farEndInvocation() {
	return farEndNode()->invocationNumber(); }

////////////////////////////
// EGGateList methods //
////////////////////////////

void EGGateLink :: removeMeFromList() { 
	if (myList) myList->removeLink(this); }

// This routine searches the precedence list for the first
// gate belonging to "master". A pointer to the first
// prec gate for "master" is returned; null is returned if
// master does not exist in the list.
EGGate* EGGateList::findMaster(DataFlowStar *master) {
	EGGateLinkIter iter(*this);  
	EGGate *q;
	while ((q=iter++)!=0) {
		if (q->farEndMaster() == master) break;
	}
	return q;
}



// This routine searches the precedence list for the node, NEXT TO 
// WHICH "node" should be placed. If the new node should be placed 
// AFTER the returned node, ret is set to +1; if the node should go
// BEFORE, ret is set to -1; if no insertion is necessary
// (an entry for the same invocation & delay is found), ret is
// set to 0; and if no entries having this master are found, then ret is 
// to be ignored and the function returns a null pointer.

EGGateLink* EGGateList::findInsertPosition (EGNode *node, int delay, int& ret)
{

	DataFlowStar *master = node->myMaster();
	int invocation = node->invocationNumber();

	EGGate *p = findMaster(master);
	if (p == 0) {   // no entries having this master
		return 0;
	}

	EGGateLink *prev = 0;
	EGGateLink *cur = p->getLink();
	while (cur != 0) {
		p = cur->gate();
		if (p->farEndMaster() != master) {
			ret = 1;
			return prev;
		}      

		if (p->farEndInvocation() == invocation) {
			while (cur != 0) {
				p = cur->gate();
				if (p->farEndInvocation() != invocation) {
					ret=1;
					return prev;
				}
				if (p->farEndMaster() != master) {
					ret=1;
					return prev;
				}
				if (p->delay() > delay) {
					ret=-1;
					return cur;
				} 
				if (p->delay()== delay) {
					ret=0;
					return cur;
				}
				prev=cur;
				cur = cur->nextLink();
			}
			ret=1;
			return prev; 
		}

		if (p->farEndInvocation() > invocation) {
			ret=-1;
			return cur;
		}
		prev = cur;
		cur = cur->nextLink();
	}  
	ret = 1;
	return prev;  
} // findInsertPosition()

EGGateList::~EGGateList () {
	initialize();
}

StringList EGGateList::printMe() {
	EGGateLinkIter next_pnode(*this);
	EGGate *p;
	StringList out;
	while((p = next_pnode++)!=0) {
		out += "-- list of gates -- \n";
		out += p->printMe();
		out += "-- end of list -- \n";
	}
	return out;
}



// Insert the argument precedence node into the list.  Return a pointer 
// to the node after it's inserted.
// Side effect : if there is already a link to the same EGNode, with 
// the same delay, "pnode" will be deallocated, and the number of samples 
// it had, will be added to the existing node.

void EGGateList :: insertGate(EGGate *pgate, int update) 
{
	int pos;
	EGGateLink *p = 
		findInsertPosition(pgate->farEndNode(),pgate->delay(),pos);

	EGGateLink* temp = (EGGateLink*) createLink(pgate);

	if (p == 0)  // no entries of this master yet  
		insertLink(temp);
	else {
		if (pos > 0) insertBehind(temp,p);
		else if (pos < 0) insertAhead(temp,p);
		else {
			// compare the porthole
			if (pgate->aliasedPort() != p->gate()->aliasedPort()) {
				insertBehind(temp, p);
	
			// If a link to the same EGNode, with the same delay,
			//  already exists, then simply update the number of
			// samples in the existing node, and delete this one,
			// because it is redundant. If we're updating now,
			// save the arc -- we'll use it to insert the other
			// endpoint (this is our convention).
			} else if (update) {
				p->gate()->addSamples(pgate->samples());
			} else { LOG_DEL; delete pgate; }
			return;
		}
	}  
	temp->myList = this;
}

void EGGateList::initialize() {
	EGGateLinkIter iter(*this);
	EGGate *p;
	while ((p=iter++)!=0) {
		LOG_DEL; delete p;
	}
	DoubleLinkList :: initialize();
}

EGGate* EGGateLinkIter :: nextMaster(DataFlowStar* m) {
	EGGate* tmp = next();
	if (tmp == 0) return 0;
	DataFlowStar* s = tmp->farEndMaster();
	if (s == m || s == refMaster) return nextMaster(m);
	refMaster = s;
	return tmp;
}
