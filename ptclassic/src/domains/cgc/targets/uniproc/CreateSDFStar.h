#ifndef _CGCTargetWH_h
#define _CGCTargetWH_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1995  The Regents of the University of California.
All Rights Reserved.

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

 Programmer: Jose Luis Pino, initial version based on SilageSimTarget


*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "CGCTarget.h"

class CGCTargetWH : public CGCTarget {
public:
    CGCTargetWH(const char* name, const char* starclass, const char* desc);

    int isA(const char*) const;
    /*virtual*/ void setup();
    /*virtual*/ int run();
    /*virtual*/ Block* makeNew() const;
    /*virtual*/ void wrapup();
	
    /*virtual*/ int compileCode();
    /*virtual*/ int loadCode();
    /*virtual*/ void frameCode();
    /*virtual*/ void writeCode();
    /*virtual*/ void initCodeStrings();

    /*virtual*/ void wormInputCode(PortHole&);
    /*virtual*/ void wormOutputCode(PortHole&);

    int makeCCFile();
    int compileCCCode();
    int linkFiles();
	
protected:
    int connectStar(Galaxy& galaxy);
    CodeStream starPorts;
};

#endif
