static const char file_id[] = "ClusterNodeList.cc";
/******************************************************************
Version identification:
@(#)ClusterNodeList.cc	1.5	3/2/95

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

 Programmer:  S.  Bhattacharyya, Soonhoi Ha (4/92)

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ClusterNodeList.h"
#include "LSGraph.h"

StringList ClusterNodeList::print() {
	StringList out;
	ClusterNodeListIter nodes(*this);
	LSNode *p;
	while ((p=nodes++)!=0) {
		out += p->printMe();
		out += "\n";
	}
	return out;
}

void ClusterNodeList::markAsActive()
{ 
	ClusterNodeListIter iter(*this);
	LSNode *p;

	while ((p=iter++)!=0) 
		p->markAsActive();
}



void ClusterNodeList::markAsInactive()
{ 
	ClusterNodeListIter iter(*this);
	LSNode *p;

	while ((p=iter++)!=0) 
		p->markAsInactive();
}

// Set the first and the second masters.
void ClusterNodeList :: setComponents(LSNode* n) {

	if (!first) first = n->myMaster();
	else {
		if (!second) {
			if (first != n->myMaster())
				second = n->myMaster();
		}
	}
}

ClusterNodeList :: ~ClusterNodeList() {
	LOG_DEL; delete next;
}
