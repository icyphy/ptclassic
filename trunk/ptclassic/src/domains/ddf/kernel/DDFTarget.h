/**********************************************************************
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
 The authors gratefully acknowledge the contributions of Richard Stevens
 Date of creation (simpler version): 7/20/94

 Declaration for CDFTarget, the default target to be used in the CDF
 domain. 

***********************************************************************/
#ifndef _DDFTarget_h
#define _DDFTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"
#include "StringState.h"
#include "IntState.h"
#include "FloatState.h"

class DDFTarget : public Target {
protected:

	// For the original DDF scheduler, it defines the number of
	// execution cycles to be overlapped in execution.
	IntState numOverlapped;

	// The user can specify the maximum buffer size on each arc.
	// Unbounded arc is detected at runtime by comparing the arc size
	// and this limit.
	IntState maxBufferSize;

	// schedulePeriod for interface with a timed domain.
	FloatState schedulePeriod;

public:
	DDFTarget();
	Block* makeNew() const;
	void setup();
	~DDFTarget();
	// class identification
	int isA(const char*) const;
	const char* className() const;
};
#endif
