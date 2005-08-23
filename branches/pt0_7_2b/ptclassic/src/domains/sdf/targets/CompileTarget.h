#ifndef _CompileTarget_h
#define _CompileTarget_h 1
/******************************************************************
Version identification:
@(#)CompileTarget.h	1.18	7/30/96

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

 Programmer:  E. A. Lee and J. Buck
 Date of creation: 7/14/92 (formerly there was only a .cc file)

This Target produces and compiles a standalone C++ program for
a universe.

*******************************************************************/
#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"
#include "IntState.h"
#include "Block.h"
#include "Star.h"
#include "PortHole.h"
#include "HLLTarget.h"

// Defined in SDFDomain.cc
extern const char SDFdomainName[];

class CompileTarget : public HLLTarget {
public:
	// Constructor
	CompileTarget(const char* nam, const char* stype, const char* desc,
		      const char* assocDomain = SDFdomainName);

	void setup();
	void begin();
	int run();
	void wrapup ();

	// Return a copy of itself
	/*virtual*/ Block* makeNew() const;

	// Routines for writing code: schedulers may call these
	void writeFiring(Star& s, int depth);

private:
	// Return pointer to its MultiPortHole and return itself otherwise
	const GenericPort* findMasterPort(const PortHole* p) const;

	// Return porthole name if PortHole or name.newPort() if MultiPortHole
	StringList expandedName(const GenericPort* p) const;

	// Create a sanitized C++ identifier for a star
	StringList sanitizedStarName(const Star& c) const; 

	// Replace quotation marks with quoted quotation marks (i.e. " => \")
	StringList quoteQuotationMarks(const char* str);

	StringList tcltkSetup();

	StringList tcltkInitialize(StringList& universeName);

	StringList galDef(Galaxy* galaxy, StringList className, int level);

	int writeGalDef(Galaxy& galaxy, StringList className);

	// Pathname of the log file for schedulers
	StringList logPath;
};

#endif
