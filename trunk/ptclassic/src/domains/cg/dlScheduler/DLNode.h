#ifndef _DLNode_h
#define _DLNode_h
#ifdef __GNUG__
#pragma interface
#endif

#include "ParNode.h"
#include "StringList.h"
#include "CGStar.h"

/*****************************************************************
Version identification:
$Id$	

Copyright (c) 1991 The Regents of the University of California.
			All Rights Reserved.

Programmer: Soonhoi Ha
Date of last revision: 

*****************************************************************/

                //////////////////////
                //   class DLNode   //
                //////////////////////

// This class represents a node in the precedence graph.  While
// it is derived from the expanded graph node, it contains additional
// data members which are used for Quasi-scheduling.

class DLNode : public ParNode {

public: 
	// Constructor declaration
	DLNode(DataFlowStar* Mas, int invoc_no);

	// Constructor used for idle nodes or communication nodes
	DLNode(int type);

	// redefine.
	int getLevel() 
		{ return StaticLevel + ((CGStar*) myMaster())->maxComm(); }
};

#endif
