#ifndef _DyConstruct_h
#define _DyConstruct_h 1
#ifdef __GNUG__
#pragma interface
#endif

#include "DataStruct.h"
#include "PortHole.h"
#include "Galaxy.h"

/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer:  Soonhoi Ha

 Base class of the dynamic constructs.
 The virtual methods in this class should be redefined in the derived
 classes.

**************************************************************************/

class CGMacroClusterGal;
class CGStar;

class DyConstruct {

public:
	// check the topology
	virtual int checkTopology(CGMacroClusterGal*);

	// initialize the schedule lists
	virtual void clearSchedules();

	// constructor
	DyConstruct() { schedules = NULL; }

	// destructor
#if defined(__GNUG__) && __GNUG__ == 1
	// g++ 1.x has a problem with multiple inheritance from two
	// classes with virtual destructors.
#else
	virtual
#endif
	~DyConstruct();
protected:
	// schedule lists
	SequentialList* schedules;

	// control PortHoles to be examined.
	PortHole* testPort;

	// compare type of ddf star and the argument type
	int compareType(CGStar&, const char*);

};

#endif
