static const char file_id[] = "MacroGraph.cc";

/*****************************************************************
Version identification:
@(#)MacroGraph.cc	1.3	1/1/96

Copyright (c) 1995 Seoul National University
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

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "MacroGraph.h"
#include "CGMacroCluster.h"
#include "MultiTarget.h"
#include "EGGate.h"
#include "ConstIters.h"
#include "KnownBlock.h"

// redefine the virtual methods
EGNode *MacroGraph :: newNode(DataFlowStar* s, int i)
	{ LOG_NEW; return new MacroNode(s,i); }

                        /////////////////////////
                        ///  initializeGraph  ///
                        /////////////////////////

int MacroGraph :: initializeGraph() {
	// first compute the critical path of each node
	ParGraph :: initializeGraph();

	// now compute the urgency of each node
	EGIter nxtNod(*this);
	MacroNode* node;

	// Visit each node in the expanded graph
	while ((node = (MacroNode*) nxtNod++) != 0) {
		workAfter(node);
	}
	return TRUE;
}
	

                        ////////////////////
                        ///  resetGraph  ///
                        ////////////////////

void MacroGraph :: resetGraph() {

	DLGraph :: resetGraph();
	unschedWork  = ExecTotal;
}

			/////////////////////
			///  workAfterMe  ///
			/////////////////////

// compute the sum of all execution time of the descendents of the node(pd).
int multiPathWork(MacroNode* pd);

int MacroGraph :: workAfter(MacroNode* pd)
{
	if (pd->workAfterMe() > 0) return pd->workAfterMe();
	int numDesc = pd->descendants.size();
	if (numDesc == 1) {
		EGGateLinkIter desciter(pd->descendants);
		EGGate* g = desciter++;
		MacroNode* n = (MacroNode*) g->farEndNode();
		return pd->setDescWork(pd->getExTime() + workAfter(n));
	} else if (numDesc == 0) {
		return pd->setDescWork(pd->getExTime());
	}

        // reset the busy flag of the ParNode.
        EGIter nxtNod(*this);   // Attach an iterator to the MacroGraph
        ParNode *node;

        // Visit each node in the expanded graph
        while ((node = (ParNode*)nxtNod++) != 0) {
		node->resetVisit();
	}
	
	return pd->setDescWork(multiPathWork(pd));
}

int multiPathWork(MacroNode* pd) {

	EGGateLinkIter desciter(pd->descendants); // iterator for descs
	EGGate *dflink;
	MacroNode* node;
	int total = 0;

	pd->beingVisited();

	// iterate for descendents.
	while ((dflink = desciter++) != 0) {
		node = (MacroNode*) dflink->farEndNode();
		if (node->alreadyVisited()) continue;
	        total += multiPathWork(node);
	}
	total += pd->getExTime();
	return total;
}

// clone a star
DataFlowStar* MacroNode :: copyStar(CGTarget* t, int pid, int flag) {
	CGMacroCluster* org = (CGMacroCluster*) myMaster();
	DataFlowStar* newS;
	if (org->asSpecialBag()) {
		int ix = pf->profileIx(invocationNumber(),pid);
		MultiTarget* mtarget = (MultiTarget*) t->parent();
		return mtarget->createMacro(org,ix,invocationNumber(),flag);
	}
                 
	CGAtomCluster* as = (CGAtomCluster*) org;
	CGStar& realS = as->real();
	if (!t->support(&realS)) {
		// FIXME: need documentation.  Are we remapping the
		// star into a different domain here?
		// FIXME: assumes target()->starType() ends in "Star"
		// and is preceded by the domain.
		// This needs to be fixed!
		const char* foo = t->starType();
		// FIXME: assumes 30-letter max starType name!!!
		char temp[30];
		strcpy(temp, foo);
		int len = strlen(foo);
		temp[len-4] = 0;
		// Pointer arithmetic to strip off domain name in a star name
		// if realS.className = "SDFRamp" than sname = "Ramp"
		const char* sname = realS.className() + strlen(realS.domain());
		newS = (DataFlowStar*) KnownBlock :: clone(sname,temp);
	} else {
		newS = (DataFlowStar*) realS.clone();
	}
 
	CBlockStateIter nexts(realS);
	const State* srcStatePtr; 
	State *destStatePtr;    
	while ((srcStatePtr = nexts++) != 0) {
		destStatePtr = newS->stateWithName(srcStatePtr->name());
		StringList temp = srcStatePtr->currentValue();
		destStatePtr->setInitValue(hashstring(temp)); 
	}

	return newS;
}
   
const char* MacroNode :: readRealName() {
	CGMacroCluster* s = (CGMacroCluster*) myStar();
	if (!s) return 0;
	return s->realName();
}
