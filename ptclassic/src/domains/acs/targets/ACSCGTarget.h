// This may look like C code, but it is really -*- C++ -*-
#ifndef _ACSCGTarget_h
#define _ACSCGTarget_h

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


 Programmers:  Eric Pauer (Sanders)
 Date of creation: 1/15/98
 Version: @(#)ACSCGTarget.h	1.6 05/15/98

***********************************************************************/

#include "ACSTarget.h"
#include "SDFScheduler.h"
#include "StringState.h"
#include "IntState.h"

class ACSCGTarget : public ACSTarget {
public:
    ACSCGTarget(const char* name, const char* starclass,const char* desc,
            const char* category = NULL, const char* assocDomain = ACSdomainName);

    ~ACSCGTarget();

    // Class identification.
    /*virtual*/ int isA(const char*) const;

    // Return a copy of itself
    /* virtual */ Block* makeNew() const = 0;

    // Routines for writing code: schedulers may call these
    /*virtual*/ void beginIteration(int repetitions, int depth);
    /*virtual*/ void endIteration(int repetitions, int depth);

    // return a name that can be used as C++ identifiers, derived
    // from the actual name.
    StringList sanitize(const char* s) const;
    StringList sanitizedName(const NamedObj &b) const;
    virtual StringList sanitizedFullName(const NamedObj &b) const;

protected:
    // Add code to the beginning of a CodeStream instead of the end.
    void prepend(StringList code, CodeStream& stream);

};

#endif //_ACSCGTarget_h
