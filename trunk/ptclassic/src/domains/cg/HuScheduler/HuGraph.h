#ifndef _HuGraph_h
#define _HuGraph_h
#ifdef __GNUG__
#pragma interface
#endif

#include "DLGraph.h"
#include "HuNode.h"

/****************************************************************
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
                //   class HuGraph   //
                //////////////////////

// This class stores the precedence graph which is scheduled by
// Hu's level scheduler.  It consists of HuNodes (derived from ParNodes) 
// connected through a subset of the arcs of the ExpandedGraph.

class HuGraph : public DLGraph {

public:
	// reset the graph for new schedules.
	// initialize runnableNodes.
	void resetGraph();

	// find a runnable block whose execution time is closest to
	// the given limit.
	HuNode* findTinyBlock(int limit);

	// return the smallest execution time among the runnable nodes.
	int smallestExTime();
	int getMinWork() { return minWork; }

	// redefine these virtual allocator to allocate DL Nodes.
	EGNode *newNode(DataFlowStar*, int);

private:
	// Minimum execution time among all runnable nodes.
	int minWork;
};

#endif
