#ifndef _QSNode_h
#define _QSNode_h
#ifdef __GNUG__
#pragma interface
#endif

#include "ParNode.h"

/*****************************************************************
Version identification:
$Id$	

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

                //////////////////////
                //   class QSNode   //
                //////////////////////

// This class represents a node in the precedence graph.  While
// it is derived from the expanded graph node, it contains additional
// data members which are used for Quasi-scheduling.

class QSNode : public ParNode {

public: 
	// Constructor declaration
	QSNode(SDFStar* Mas, int invoc_no);

	// Constructor used for idle nodes or communication nodes
	QSNode(int type);

	// Indicate that this node has been scheduled.
	void resetAssignedFlag(int i) { assignedFlag = i; }
	void setAssignedFlag() { assignedFlag++; }
	int alreadyAssigned() { return (assignedFlag > 0)? TRUE : FALSE; }

private:
	// Indicate whether or not this node has been assigned.
	int assignedFlag;
};

#endif
