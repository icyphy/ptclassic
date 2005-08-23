#ifndef _TkSchedTarget_h
#define _TkSchedTarget_h 1
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

 Programmer: Michael C. Williamson

 VHDL target for interactive architecture planning.

*******************************************************************/

#ifdef __GNUG__
#pragma interface
#endif

#include "SynthArchTarget.h"
#include "VHDLStar.h"
#include "VHDLCompDecl.h"
#include "VHDLSignal.h"
#include "VHDLState.h"
#include "VHDLCluster.h"
#include "VHDLFiring.h"
#include "Attribute.h"

class TkSchedTarget : public SynthArchTarget {
public:
	TkSchedTarget(const char*, const char*, const char*);
	/*virtual*/ Block* makeNew() const;

	// Class identification.
	/*virtual*/ int isA(const char*) const;

	// Method to bring up interactive graph app.
	/*virtual*/ void interact();

	// redefine writeCode: default file is "code.vhd"
	/*virtual*/ void writeCode();

	// Compile, run the code.
	/*virtual*/ int compileCode();

	// Run the code.
	/*virtual*/ int runCode();

protected:
	/*virtual*/ void begin();

	/*virtual*/ void setup();

private:

};

#endif
