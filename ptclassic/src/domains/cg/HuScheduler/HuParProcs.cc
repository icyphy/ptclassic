static const char file_id[] = "HuParProcs.cc";

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

#include "HuParProcs.h"
#include "EGGate.h"
#include "StringList.h"
#include "Profile.h"
#include "HuNode.h"
#include "Error.h"

// constructor
HuParProcs :: HuParProcs(int pNum, MultiTarget* t) : DLParProcs(pNum,t) {}

/*****************************************************************
		SCHEDULE AIDS
 ****************************************************************/

void HuParProcs :: fireNode(DLNode* temp) 
{
	int pix = temp->getProcId();

	EGGateLinkIter nextKid(temp->descendants);
	EGGate* d;
	while ((d = nextKid++) != 0) {
		HuNode* pd = (HuNode*) d->farEndNode();
		if (pd->fireable()) {
			pd->setAvailTime();
			pd->setPreferredProc(pix);
			myGraph->sortedInsert(myGraph->runnableNodes,pd,1);
		}
	}
}

/*****************************************************************
		Scheduling ATOMIC block
 ****************************************************************/

// schedule a normal atomic block.
void HuParProcs :: scheduleSmall(DLNode* node)
{
	HuNode* pd = (HuNode*) node;

	// get the earliest available processor or the designated processor
	// If the processor is not available at the designated time,
	// put the node to the runnable node list with the next
	// available time.
	int canProc = -1;
	int earliest;
	int targetTime = pd->availTime();
        if (pd->atBoundary()) {
		earliest = costAssignedTo(pd, 0, targetTime);
                if (earliest == targetTime) canProc = 0;

        } else if (pd->sticky() && pd->invocationNumber() > 1) {
                ParNode* firstN = (ParNode*) pd->myMaster()->myMaster();
                int pix = firstN->getProcId();
		earliest = costAssignedTo(pd, pix, targetTime);
                if (earliest == targetTime) canProc = pix;
        } else {
                candidate = mtarget->candidateProcs(this, node->myMaster());
		// get the canProc or the earliest available time
		int pix = pd->getPreferredProc();
		earliest = costAssignedTo(pd, pix, targetTime);
		if (earliest == targetTime) {
			canProc = pix;
		} else {
			int bound = candidate->size();
			for (int i = 0; i < bound; i++) {
				pix = candidate->elem(i);
				int cost = costAssignedTo(pd, pix, targetTime);
				if (cost == targetTime) {
					canProc = pix;
					break;
				} else if (cost < earliest) {
					earliest = cost;
				}
			}
		}
        }

	if (candidate->size() == 0) {
		Error::abortRun(*pd->myMaster(), " is not supported.",
			" please check resource constraits");
	}

	// check whether canProc is set or not.
	// If set, schedule the node. If not, put this node back to the 
	// runnable node list.
	if (canProc >= 0) {
		assignNode(pd, canProc, targetTime);
		fireNode(pd);

		// renew the states of the graph
		myGraph->decreaseNodes();
		myGraph->decreaseWork(pd->getExTime());
	} else {
		pd->setAvailTime(earliest);
		myGraph->sortedInsert(myGraph->runnableNodes, pd, 1);
	}
}

void HuParProcs :: scheduleIPC(int) {}
void HuParProcs :: prepareComm(DLNode*) {}

/*****************************************************************
		Scheduling BIG block
 ****************************************************************/

int HuParProcs :: determinePPA(DLNode* node, IntArray& avail) {
	HuNode* pd = (HuNode*) node;

	// examine candidate processors
	candidate = mtarget->candidateProcs(this, node->myMaster());

	// decide the starting processor assigned to the construct.
	int earliest;
	int optId = decideStartingProc(pd, &earliest);
	
	if (optId < 0) {
		pd->setAvailTime(earliest);
		myGraph->sortedInsert(myGraph->runnableNodes, pd, 1);
		return -1;
	} 

	// sort the processor indices with available time.
	sortWithAvailTime(earliest);

	// fill out the array.
	for (int i = numProcs-1; i >= 0; i--) {
		int temp = getProc(pIndex[i])->getAvailTime();
		if (temp > earliest) {
			avail[i] = temp - earliest;     // positive.
		} else {
			if (pIndex[i] == optId) {
				int t = pIndex[0];
				pIndex[0] = optId;
				pIndex[i] = t;
				temp = getProc(t)->getAvailTime();
			}
			avail[i] = temp - earliest;     // non-positive.
		}
	}
	return earliest;
}

int HuParProcs :: decideStartingProc(DLNode* node, int* earliest) {

	HuNode* pd = (HuNode*) node;

	int targetTime = pd->availTime();
	int canProc = -1;

	int pix = pd->getPreferredProc();
	*earliest = getProc(pix)->getAvailTime();
	if ((*earliest) <= targetTime) {
		canProc = pix;
	} else {
		int bound = candidate->size();
		for (int i = 0; i < bound; i++) {
			pix = candidate->elem(i);
			int cost = getProc(pix)->getAvailTime();
			if (cost <= targetTime) {
				canProc = pix;
				break;
			} else if (cost < *earliest) {
				*earliest = cost;
			}
		}
	}
	return canProc;
}

