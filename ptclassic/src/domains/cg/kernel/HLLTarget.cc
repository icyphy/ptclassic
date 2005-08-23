/******************************************************************
Version identification:
@(#)HLLTarget.cc	1.17	7/30/96

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

 Programmer:  E. A. Lee
 Date of creation: 12/8/91

This Target produces and compiles a standalone C++ program for
a universe.

*******************************************************************/
static const char file_id[] = "HLLTarget.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "HLLTarget.h"
#include "KnownTarget.h"
#include "CGUtilities.h"
#include "pt_fstream.h"
#include <ctype.h>

// constructor
HLLTarget::HLLTarget(const char* nam, const char* startype,
		     const char* desc, const char* assocDomain) :
CGTarget(nam, startype, desc, assocDomain)
{
}

// Routines for writing code: schedulers may call these
void HLLTarget::beginIteration(int repetitions, int depth) {
	myCode << indent(depth);
	if (repetitions == -1)          // iterate infinitely
		myCode << "while(1) {\n";
	else {
	    myCode << "int " << targetNestedSymbol.push("i") << "; "
	           << "for (" << targetNestedSymbol.get() << "=0; "
		   << targetNestedSymbol.get() << " < " << repetitions << "; "
		   << targetNestedSymbol.pop() << "++) {\n";
	}
}

void HLLTarget::endIteration(int, int depth) {
	myCode << indent(depth) << "}\n";
}

// This method creates a name derived from the name of the object
// where all characters in the name that are not alphanumeric are
// changed to '_' so that the resulting name is a legitimate C or C++
// identifier.  For all names that begin with a numeric character,
// the character 'x' is prepended.
StringList HLLTarget :: sanitizedName (const NamedObj& obj) const {
        const char *s = obj.name();
	return sanitize(s);
}


StringList HLLTarget :: sanitize(const char* s) const {
        const char *snm = ptSanitize(s);
        StringList out(snm);
        return out;
}

StringList HLLTarget :: sanitizedFullName (const NamedObj& obj) const {
        const char *snm = ptSanitize(obj.fullName());
        StringList out(snm);
        return out;
}

ISA_FUNC(HLLTarget,CGTarget);
