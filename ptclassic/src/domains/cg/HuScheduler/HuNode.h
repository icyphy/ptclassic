#ifndef _HuNode_h
#define _HuNode_h
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
                //   class HuNode   //
                //////////////////////

// This class represents a node in the precedence graph.  While
// it is derived from the expanded graph node, it contains additional
// data members which are used for Hu-level scheduling.

class HuNode : public ParNode {

public: 
	// Constructor declaration
	HuNode(DataFlowStar* Mas, int invoc_no);

	// Constructor used for idle nodes or communication nodes
	HuNode(int type);

	// Indicate that this node has been scheduled.
	void resetAssignedFlag(int i) { assignedFlag = i; }
	void setAssignedFlag() { assignedFlag++; }
	int alreadyAssigned() { return (assignedFlag > 0)? TRUE : FALSE; }

	// get and set the preferredProc
	int getPreferredProc() { return preferredProc; }
	void setPreferredProc(int i) { preferredProc = i; }

private:
	// Indicate whether or not this node has been assigned.
	int assignedFlag;
	// preferred proc id.
	int preferredProc;
};

#endif
