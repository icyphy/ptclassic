// This may look like C code, but it is really -*- C++ -*-
#ifndef _CGCMakefileTarget_h
#define  _CGCMakefileTarget_h 1

/******************************************************************
Version identification:
$Id$

Copyright (c) %Q% The Regents of the University of California.
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

 Programmer: Christopher Hylands
 Based on the MultiTarget by Soonhoi Ha

 This is a Makefile target class for CGCdomain.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "StringState.h"
#include "CGCTarget.h"

class EventHorizon;
class CGCTarget;


class CGCMakefileTarget : public CGCTarget {
public:
	CGCMakefileTarget(const char* name, const char* starclass,
			  const char* desc);
	/* virtual */ Block* makeNew() const;

	~CGCMakefileTarget();

	void writeCode();
  	int compileCode();

	int isA(const char*) const;

private:

	StringState skeletonMakefile;
	IntState appendToMakefile;
};

#endif
