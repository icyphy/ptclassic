static const char file_id[] = "ACSCGTarget.cc";
/**********************************************************************
Copyright (c) 1998 The Regents of the University of California.
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


 Programmers:  Eric Pauer (Sanders), Christopher Hylands, Edward A. Lee
 Date of creation: 1/15/98
 Version: @(#)ACSCGTarget.cc	1.5 05/15/98

***********************************************************************/


#include "ACSCGTarget.h"
#include "KnownTarget.h"
#include "CGUtilities.h"
#include "pt_fstream.h"
#include <ctype.h>


ACSCGTarget::ACSCGTarget(const char* name,const char* starclass,
        const char* desc, const char* category, const char* assocDomain)
    : ACSTarget(name,starclass,desc, category, assocDomain) {}

ACSCGTarget :: ~ACSCGTarget() {}

// Routines for writing code: schedulers may call these
void ACSCGTarget::beginIteration(int repetitions, int depth) {
// JMS
return;
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

void ACSCGTarget::endIteration(int, int depth) {
	myCode << indent(depth) << "}\n";
}

// This method creates a name derived from the name of the object
// where all characters in the name that are not alphanumeric are
// changed to '_' so that the resulting name is a legitimate C or C++
// identifier.  For all names that begin with a numeric character,
// the character 'x' is prepended.
StringList ACSCGTarget :: sanitizedName (const NamedObj& obj) const {
        const char *s = obj.name();
	return sanitize(s);
}


StringList ACSCGTarget :: sanitize(const char* s) const {
        const char *snm = ptSanitize(s);
        StringList out(snm);
        return out;
}

StringList ACSCGTarget :: sanitizedFullName (const NamedObj& obj) const {
        const char *snm = ptSanitize(obj.fullName());
        StringList out(snm);
        return out;
}

// Add code to the beginning of a CodeStream instead of the end.
void ACSCGTarget :: prepend(StringList code, CodeStream& stream)
{
    StringList temp;
    temp << code;
    temp << stream;
    stream.initialize();
    stream << temp;
}



ISA_FUNC(ACSCGTarget,ACSTarget);
