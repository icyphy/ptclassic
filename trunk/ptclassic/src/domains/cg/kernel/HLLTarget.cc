static const char file_id[] = "HLLTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 12/8/91

This Target produces and compiles a standalone C++ program for
a universe.

*******************************************************************/

#include "HLLTarget.h"
#include "KnownTarget.h"
#include "pt_fstream.h"
#include <ctype.h>

// constructor
HLLTarget::HLLTarget(const char* nam, const char* startype,
	const char* desc) : CGTarget(nam, startype, desc)
{
}

void HLLTarget::wrapup() {
    writeCode();
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
        LOG_NEW; char *snm = new char [strlen(s) + 2];
        char *n = snm;
	if(isdigit(*s)) *(n++) = 'x';
        while (*s != 0) {
            if(isalnum(*s)) *(n++) = *(s++);
            else { *(n++) = '_'; s++; }
        }
        *n = 0;
        StringList out(snm);
        LOG_DEL; delete snm;
        return out;
}

StringList HLLTarget :: sanitizedFullName (const NamedObj& obj) const {
        StringList out;
        if(obj.parent() != NULL) {
		Block* b = obj.parent();
		if (b->isItWormhole() == 0) {
                	out = sanitizedFullName(*obj.parent());
                	out += ".";
		}
                out += sanitizedName(obj);
        } else {
                out = sanitizedName(obj);
        }
        return out;
}
