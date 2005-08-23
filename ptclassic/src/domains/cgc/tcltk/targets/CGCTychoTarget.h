#ifndef _CGCTychoTarget_h
#define _CGCTychoTarget_h 1
/******************************************************************
Version identification:
@(#)CGCTychoTarget.h	1.7 04/09/97

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer: Sunil Bhave

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "StringList.h"
#include "StringState.h"
#include "CGCTarget.h"

class CGCTychoTarget : public CGCTarget {
public:
	CGCTychoTarget(const char* name, const char* starclass,
		       const char* desc);
	Block* makeNew() const;

	// Combine all sections of code;
	/*virtual*/ void frameCode();
 
  	int compileCode();
	int runCode();

protected:
        StringState scriptFile;

	// generate the code for the main loop.
	/*virtual*/ CodeStream mainLoopBody();

	// code generation init routine; compute offsets, generate initCode
	int codeGenInit();
	
	// Parse variable declarations to add "static"
        void addStaticDecls( StringList &, const char * );
	
        CodeStream mainLoopTerm;
        CodeStream tychoSetup;
	CodeStream tkSetup;

        // virtual function to initialize strings
        /*virtual*/ void initCodeStrings();

};

#endif


