#ifndef _SynthTarget_h
#define _SynthTarget_h 1
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990-1994 The Regents of the University of California.
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

 Programmer: Michael C. Williamson

 Target for VHDL code generation and handoff to Synopsys synthesis.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "StructTarget.h"
#include "VHDLStar.h"
#include "VHDLCompDecl.h"
#include "VHDLCompMap.h"
#include "VHDLSignal.h"
#include "VHDLArc.h"
#include "VHDLState.h"
#include "VHDLPortVar.h"
#include "VHDLCluster.h"
#include "VHDLFiring.h"
#include "Attribute.h"
#include "IntState.h"

class SynthTarget : public StructTarget {
public:
	SynthTarget(const char* name, const char* starclass, const char* desc);
	/*virtual*/ Block* makeNew() const;

	// Class identification.
	/*virtual*/ int isA(const char*) const;

	// redefine writeCode: default file is "code.vhd"
	/*virtual*/ void writeCode();

	// Compile, run the code.
	/*virtual*/ int compileCode();

	// Run the code.
	/*virtual*/ int runCode();

protected:
	// States.
	IntState analyze;
	IntState elaborate;
	IntState compile;
	IntState report;
	
private:

};

#endif
