static const char file_id[] = "QSNode.cc";

/*****************************************************************
Version identification:
$Id$

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision:

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "QSNode.h"
                        ///////////////////////
                        ///  *Constructor*  ///
                        ///////////////////////

QSNode::QSNode(SDFStar* Mas, int invoc_no) : ParNode(Mas, invoc_no)
{
	assignedFlag = 0;
	preferredProc = 0;
}

// Alternate constructor for idle nodes and communication nodes
// If type = 1, it is an idle node.
// If type = -1, it is a send node, type = -2 indicates a receive node

QSNode::QSNode(int t) : ParNode(t) {
	assignedFlag = 0;
	preferredProc = 0;
}

