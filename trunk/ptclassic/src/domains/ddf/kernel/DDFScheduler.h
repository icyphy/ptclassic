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


 Programmers: Soonhoi Ha, Joe Buck
 The authors gratefully acknowledge the contributions of Richard Stevens.
 Date of creation: 7/20/94

 Methods for the DDF Scheduler

***********************************************************************/
#ifndef _DDFScheduler_h
#define _DDFScheduler_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "DynDFScheduler.h"

	////////////////////////////
	// DDFScheduler
	////////////////////////////

class DDFScheduler : public DynDFScheduler {
public:
	// my domain
	const char* domain() const ;

	// reset "restructured" flag for DDFSelf star
	void resetFlag() {
		DynDFScheduler::resetFlags();
	}
};

#endif
