static const char file_id[] = "HuParProcs.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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
		if(!candidateProcs(node->myMaster())) {
		   Error::abortRun(*node->myMaster(), " is not supported.",
		   " please check resource constraints.");
		   return;
		}

		// get the canProc or the earliest available time
		int pix = pd->getPreferredProc();
		earliest = costAssignedTo(pd, pix, targetTime);
		if ((earliest == targetTime) && (isCandidate(pix))) {
			canProc = pix;
		} else {
			int bound = candidate.size();
			for (int i = 0; i < bound; i++) {
				pix = candidate.elem(i);
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

	// check whether canProc is set or not.
	// If set, schedule the node. If not, put this node back to the 
	// runnable node list.
	if (canProc >= 0) {
		assignNode(pd, canProc, targetTime);
		fireNode(pd);

		// renew the states of the graph
		myGraph->decreaseNodes();
	} else {
		pd->setAvailTime(earliest);
		myGraph->sortedInsert(myGraph->runnableNodes, pd, 1);
	}
}

void HuParProcs :: scheduleIPC(int) {}
void HuParProcs :: prepareComm(DLNode*) {}

int HuParProcs :: isCandidate(int proc) {
	
	int isAcandidate = 0;
	
	for (int i = 0; i < candidate.size(); i++) 
		if (candidate.elem(i) == proc) isAcandidate = 1;
	return isAcandidate;
}
