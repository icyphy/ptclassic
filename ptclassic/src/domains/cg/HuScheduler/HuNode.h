#ifndef _HuNode_h
#define _HuNode_h
#ifdef __GNUG__
#pragma interface
#endif

#include "ParNode.h"

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
