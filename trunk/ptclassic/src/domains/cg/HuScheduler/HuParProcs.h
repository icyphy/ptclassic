#ifndef _HuParProcs_h
#define _HuParProcs_h

#ifdef __GNUG__
#pragma interface
#endif

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

#include "NamedObj.h"
#include "HuGraph.h"
#include "DLParProcs.h"

/////////////////////////
// class HuParProcs //
/////////////////////////

// A class for managing the parallel processor schedules.

class HuParProcs : public DLParProcs {

public:
	HuParProcs(int pNum, MultiTarget* t);

	/* virtual */ void scheduleSmall(DLNode*);

  	// determine the pattern of processor availability before scheduling
  	// a dynamic construct (or non-atomic node).
  	/* virtual */ int determinePPA(DLNode*, IntArray&);

protected:
	// Fire a node. Check the runnability of descendants.
	/* virtual */ void fireNode(DLNode*);

private:
	// redefine virtual methods
	/* virtual */ void prepareComm(DLNode*);	//  do nothing
	/* virtual */ void scheduleIPC(int);	//  do nothing

	// Among candidate processors, choose a processor that can execute
	// the node earliest.
	/* virtual */ int decideStartingProc(DLNode*, int*);
};

#endif
