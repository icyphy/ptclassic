static const char file_id[] = "EGGate.cc";

/******************************************************************
Version identification:
@(#)EGGate.cc	1.16	3/5/96

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

 Programmer:  Soonhoi Ha, based on S.  Bhattacharyya's code.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "EGGate.h"
#include "EGNode.h"
#include "SDFStar.h"

StringList EGGate::printMe() {
	StringList out = "nsam = ";
	out << samples() << ", and delay = " << delay() << "\n";
	return out;
}

void EGGate::allocateArc(EGGate* dest, int no_samples, int no_delays) {
	far = dest;
	dest->far = this;
	LOG_DEL; delete dynarc;
	LOG_NEW; dynarc = new EGArc(no_samples, no_delays);
	arc = dynarc;
	dest->arc = arc; 
}

// disconnect this node and it's far end node, fix the gate pointers and
// deallocate this gate.  The far gate will be deallocated by the far EGNode
EGGate::~EGGate() 
{
  if (myLink) myLink->removeMeFromList();
  if (far) { 
    far->far = 0;
    far->arc = 0;
    far = 0;
  }
  LOG_DEL; delete dynarc; 
}

void EGGate::hideMe(int flag) {
	if (!flag) far->hideMe(1);
	if (myLink) myLink->removeMeFromList();
	parent->hiddenGates.insertGate(this, 0);
}

DataFlowStar* EGGate :: farEndMaster() {
	return far ? farEndNode()->myMaster() : (DataFlowStar*)0;
}

int EGGate :: farEndInvocation() {
	return far ? farEndNode()->invocationNumber() : 0;
}

////////////////////////////
// EGGateList methods //
////////////////////////////

void EGGateLink :: removeMeFromList() { 
	if (myList) myList->removeLink(this);
}

// This routine searches the precedence list for the first
// gate belonging to "master". A pointer to the first
// prec gate for "master" is returned; null is returned if
// master does not exist in the list.
EGGate* EGGateList::findMaster(DataFlowStar *master) {
	EGGateLinkIter iter(*this);  
	EGGate *q;
	while ((q = iter++)!=0) {
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
	if (node == 0) return 0;

	DataFlowStar* master = node->myMaster();
	int invocation = node->invocationNumber();

	// If no entries having this master, then return a null pointer
	EGGate* p = findMaster(master);
	if (p == 0) return 0;

	EGGateLink* prev = 0;
	EGGateLink* cur = p->getLink();
	while (cur) {
		p = cur->gate();
		if (p->farEndMaster() != master) {
			ret = 1;
			return prev;
		}      

		if (p->farEndInvocation() == invocation) {
			while (cur) {
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
		out << "-- list of gates -- \n"
		    << p->printMe()
		    << "\n-- end of list -- \n";
	}
	return out;
}

// Insert the argument precedence node into the list.  Return a pointer 
// whether or not a node was inserted.
// Side effect : if there is already a link to the same EGNode, with 
// the same delay, "pnode" will be deallocated, and the number of samples 
// it had, will be added to the existing node.

int EGGateList :: insertGate(EGGate* pgate, int update) 
{
	int pos = 0;
	EGGateLink* p = findInsertPosition(pgate->farEndNode(),
					   pgate->delay(), pos);
	EGGateLink* temp = (EGGateLink*)createLink(pgate);

	// no entries of this master yet  
	if (p == 0)
		insertLink(temp);
	else if (pos > 0)
		insertBehind(temp, p);
	else if (pos < 0)
		insertAhead(temp, p);
	// compare the porthole
	else if (pgate->aliasedPort() != p->gate()->aliasedPort())
		insertBehind(temp, p);
	// If a link to the same EGNode, with the same delay,
	// already exists, then simply update the number of
	// samples in the existing node, and delete this one,
	// because it is redundant. If we're updating now,
	// save the arc -- we'll use it to insert the other
	// endpoint (this is our convention).
	else {
		if (update) p->gate()->addSamples(pgate->samples());
		LOG_DEL; delete temp;		// destroy the new link
		pgate->setLink(0);		// reset pointer to new link
		return FALSE;
	}
	temp->myList = this;
	return TRUE;
}

void EGGateList::initialize() {
	EGGateLinkIter iter(*this);
	EGGate* p;
	while ((p = iter++) != 0) {
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
