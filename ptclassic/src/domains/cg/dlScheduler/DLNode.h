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
@(#)DLNode.h	1.5	3/2/95	

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
