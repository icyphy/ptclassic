/**********************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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


 Programmer:  Soonhoi Ha
 The authors gratefully acknowledge the contributions of Richard Stevens
 Date of creation (simpler version): 7/20/94

 Declaration for DDF Target, the default target to be used in the DDF
 domain. 

 If restructure is 1, auto-wormholization is performed.
 This is an experimental facility that automatically creates SDF wormholes
 for subsystems that consist entirely of SDF stars.  It is disabled by
 default.

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
#include "HashTable.h"

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

	// Specify whether or not to use the experimental automatic
	// restructuring of DDF systems.  This facility identifies
	// SDF subsystems and creates SDF wormholes for them.
	IntState restructure;

	StringState logFile;

public:
	DDFTarget();
	Block* makeNew() const;
	void setup();
	~DDFTarget();
	// class identification
	int isA(const char*) const;
	const char* className() const;

	// Support hints that are used to define the number of
	// firings in one iteration. A value of zero is interpreted
	// as "don't care".
	StringList hint () { return "firingsPerIteration INT 0"; }

	// To determine the value of all hints that have been
	// specified for a particular block, call this method.
	// It returns a list of "name value" pair, separated by spaces,
	// or an empty string if there is no hint for this block.
	StringList hint (const char* blockname);

	// To determine the value of a hint of a particular type
	// that has been specified for a particular block, call this
	// method. It returns a value or an empty string.
	StringList hint (const char* blockname, const char* hintname);

	// To specify a hint to a target, call this method.
	// The return value is always a null string.
	StringList hint (const char* blockname,
		   const char* name,
		   const char* value);

private:
	TextTable firings;
};
#endif
