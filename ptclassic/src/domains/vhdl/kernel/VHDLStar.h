#ifndef _VHDLStar_h
#define _VHDLStar_h 1
/******************************************************************
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

 Programmer: Edward A. Lee, Michael C. Williamson

 This is the baseclass for stars that generate VHDL language code

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGStar.h"
#include "VHDLPortHole.h"
#include "VHDLGeodesic.h"
#include "Tokenizer.h"
#include <ctype.h>

class VHDLTarget;

class VHDLStar : public CGStar {
public:
	// initialize method
	void initialize();

	// my domain
	const char* domain() const;

	// run this star
	int run();

	// class identification
	int isA(const char*) const;

	// Return the number of the current firing
	int firingNum() { return firing; }

protected:
	// access to target (cast is safe: always a VHDLTarget)
	VHDLTarget* targ() {
		return (VHDLTarget*)target();
	}

	// Expand macros that are defined for this star
        /* virtual */ StringList expandMacro(const char*, const StringList&);

	// Expand State or PortHole reference macros.
	// First form is new prototype with three arguments.
	StringList expandRef(const char*, const char*, const char*);
	// Second and third forms are redefinitions of virtual prototypes.
	/* virtual */ StringList expandRef(const char* name)
	  { return expandRef(name, "", ""); }
	/* virtual */ StringList expandRef(const char* name,
					   const char* offset)
	  { return expandRef(name, offset, ""); }

	// Form expression interspersing operators within arg list.
	StringList expandInterOp(const char*, const char*, const char*);

	// Assignment operator, depending on variable or signal
	const char* expandAssign(const char*);

	// Temproary variable reference.
	const char* expandTemp(const char*, const char*);

	// Constant variable reference.
	const char* expandDefine(const char*, const char*, const char*);

	// Update the offset read and write pointers to the porthole queues.
	void updateOffsets();

	// Various hashed strings for quick comparison.
        const char* hashBLANK;

private:
	// Maintain counter of firing number.
	// The first firing is numbered 1.
	int firing;
};

#endif
