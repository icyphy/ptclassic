static const char file_id[] = "HuNode.cc";

/*****************************************************************
Version identification:
@(#)HuNode.cc	1.8	3/2/95

Copyright (c) 1990-1995 The Regents of the University of California.
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

#include "HuNode.h"
#include "EGGate.h"

                        ///////////////////////
                        ///  *Constructor*  ///
                        ///////////////////////

HuNode::HuNode(DataFlowStar* Mas, int invoc_no) : DLNode(Mas, invoc_no)
{
	timeTBS = 0;
	preferredProc = 0;
}

// Alternate constructor for idle nodes and communication nodes
// If type = 1, it is an idle node.
// If type = -1, it is a send node, type = -2 indicates a receive node

HuNode::HuNode(int t) : DLNode(t) {
	timeTBS = 0;
	preferredProc = 0;
}

                        ///////////////////////
                        ///  setAvailTime   ///
                        ///////////////////////

void HuNode :: setAvailTime() {
	// scan the ancestors.
	EGGateLinkIter preciter(ancestors);
	EGGate* q;

	int maxT = 0;
	while((q = preciter++)!=0) {
		HuNode* p = (HuNode*) q->farEndNode();
		int fTime = p->getFinishTime();
		if (fTime > maxT) maxT = fTime;
	}
	timeTBS = maxT;
}

