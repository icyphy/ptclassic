#ifndef _DDFStar_h
#define _DDFStar_h 1
#ifdef __GNUG__
#pragma interface
#endif
/******************************************************************
Version identification:
@(#)DDFStar.h	1.20	7/19/95

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

 Programmer:  Soonhoi Ha
 Date of creation: 8/9/90

 DDFStar is a Star that runs under the DDF Scheduler.

*******************************************************************/

#include "DynDFStar.h"
#include "Particle.h"
#include "DDFPortHole.h"
#include "IntState.h"


	////////////////////////////////////
	// class DDFStar
	////////////////////////////////////

class DDFStar : public DynDFStar  {

public:
	// constructor
	DDFStar() {}

	// initialize DDF-specific members, plus std initialize
	void initialize();

	// bypass the DataflowStar :: run method
	int run();

	// class identification
	int isA(const char*) const;

	// my domain
	const char* domain() const;

	// is it recursion star?
	virtual int isItSelf();

protected:
	void initWait() { clearWaitPort();}
};

#endif
