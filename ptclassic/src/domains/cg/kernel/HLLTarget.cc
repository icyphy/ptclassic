static const char file_id[] = "BaseCTarget.cc";
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

#include "BaseCTarget.h"
#include "KnownTarget.h"
#include <ctype.h>

// constructor
BaseCTarget::BaseCTarget(const char* nam, const char* startype,
	const char* desc) : CGTarget(nam, startype, desc),
	unique(0)
{
}

void BaseCTarget::wrapup() {
    char* codeFileName = writeFileName("code.output");
    UserOutput codeFile;
    if(Scheduler::haltRequested()) return;
    if(!codeFile.fileName(codeFileName)) {
	Error::abortRun("Can't open code file for writing: ",codeFileName);
	return;
    }
    writeCode(codeFile);
}

// Routines for writing code: schedulers may call these
void BaseCTarget::beginIteration(int repetitions, int depth) {
	myCode << indent(depth);
	if (repetitions == -1)          // iterate infinitely
		myCode << "while(1) {\n";
	else {
		myCode << "int i" << unique << "; ";
		myCode << "for (i" << unique << "=0; i" << unique <<
		    " < " << repetitions << "; i" << unique++ << "++) {\n";
	}
}

void BaseCTarget::endIteration(int, int depth) {
	myCode << indent(depth) << "}\n";
}

// This method creates a name derived from the name of the object
// where all characters in the name that are not alphanumeric are
// changed to '_' so that the resulting name is a legitimate C or C++
// identifier.  For all names that begin with a numeric character,
// the character 'x' is prepended.
StringList BaseCTarget :: sanitizedName (const NamedObj& obj) const {
        const char *s = obj.readName();
	return sanitize(s);
}


StringList BaseCTarget :: sanitize(const char* s) const {
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

StringList BaseCTarget :: sanitizedFullName (const NamedObj& obj) const {
        StringList out;
        if(obj.parent() != NULL) {
                out = sanitizedFullName(*obj.parent());
                out += ".";
                out += sanitizedName(obj);
        } else {
                out = sanitizedName(obj);
        }
        return out;
}
