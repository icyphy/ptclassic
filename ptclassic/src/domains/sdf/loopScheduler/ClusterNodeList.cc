static const char file_id[] = "ClusterNodeList.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

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
