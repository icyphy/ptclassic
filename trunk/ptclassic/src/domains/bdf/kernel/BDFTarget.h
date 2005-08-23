/**********************************************************************
Version identification:
@(#)BDFTarget.h	2.9	7/30/96

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer:  J. T. Buck

 Declaration for BDFTarget, the default target to be used in the BDF
 domain.  Very similar to SDFTarget.

 BDFTarget uses a clustering scheduler (BDFClustSched).

***********************************************************************/
#ifndef _BDFTarget_h
#define _BDFTarget_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "Target.h"
#include "StringState.h"
#include "IntState.h"
#include "FloatState.h"

class BDFTarget : public Target {
public:
	// Constructor
	BDFTarget();

	// Destructor
	~BDFTarget();

	void setup();

	Block* makeNew() const;

protected:
	StringState logFile;
	IntState allowDynamic;
	IntState requireStronglyConsistent;

	// schedulePeriod for interface with a timed domain.
	FloatState schedulePeriod;

private:
	char* logPath;
};
#endif
