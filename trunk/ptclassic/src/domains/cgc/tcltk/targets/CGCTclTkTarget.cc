static const char file_id[] = "CGCTclTkTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer: E. A. Lee

 Base target for C code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCTclTkTarget.h"
#include "CGCStar.h"
#include "KnownTarget.h"

// #define PTOLEMY04

// constructor
CGCTclTkTarget::CGCTclTkTarget(const char* name,const char* starclass,
                   const char* desc) : CGCTarget(name,starclass,desc) {
	funcName.setInitValue("go");
	compileOptions.setInitValue(
		  "-I$PTOLEMY/tcl/tk3.1/src -I$PTOLEMY/tcl/tcl6.6/src "
		  "-I$PTOLEMY/src/domains/cgc/tcltk/lib");
	linkOptions.setInitValue(
		  "$PTOLEMY/tcl/tk3.1/lib/$ARCH.opt/libtk.a "
		  "$PTOLEMY/tcl/tcl6.6/lib/$ARCH.opt/libtcl.a "
		  "-L$PTOLEMY/tcl/tk3.1/lib/$ARCH.opt "
		  "-L/usr/X11/lib -lXpm -lX11 -lm");
	loopingLevel.setInitValue("1");
	addStream("mainLoopInit", &mainLoopInit);
	addStream("mainLoopTerm", &mainLoopTerm);
	addStream("tkSetup", &tkSetup);
}

void CGCTclTkTarget :: initCodeStrings() {
	CGCTarget::initCodeStrings();
	mainLoopInit.initialize();
	mainLoopTerm.initialize();
	tkSetup.initialize();
}

// clone
Block* CGCTclTkTarget :: makeNew () const {
	LOG_NEW; return new CGCTclTkTarget(name(),starType(),descriptor());
}

/////////////////////////////////////////
// codeGenInit
/////////////////////////////////////////

int CGCTclTkTarget :: codeGenInit() {

	// Define variables needed by the tcl init file.
	globalDecls += "char *name = \"";
	globalDecls += galaxy()->name();
	globalDecls += "\";\n";
	globalDecls += "int numIterations = ";
	globalDecls += (int)scheduler()->getStopTime();
	globalDecls += ";\n";
	globalDecls += "#include \"tkMain.c\"\n\n";

	mainLoopInit += "Tk_DoOneEvent(1);\n";
	mainLoopInit += "while (runFlag == -1) Tk_DoOneEvent(0);\n";
	mainLoopInit += "if (runFlag == 0) break;\n";

	mainLoopTerm += "runFlag = 0;\n";

	return CGCTarget::codeGenInit();
}

void CGCTclTkTarget :: beginIteration(int repetitions, int depth) {
	CGCTarget::beginIteration(repetitions, depth);
	myCode += "Tk_DoOneEvent(1);\n";
}

void CGCTclTkTarget :: mainLoopCode() {
        // Generate Code.  Iterate an infinite number of times if
        // target is inside a wormhole
        int iterations = inWormHole()? -1 : (int)scheduler()->getStopTime();

	// Need specialized code instead of the beginIterations method
	// so that the value can be changed each time the function is run.
	// and infinite iterations can be supported
	myCode << indent(1);
	mainDecls << indent(1)
		  << "int " << targetNestedSymbol.push("i") << ";\n";
	myCode << targetNestedSymbol.get() << "=0; " << "while ("
	       << targetNestedSymbol.pop() << "++ != " << "numIterations"
	       << ") {\n";
	myCode += wormIn;

        if (inWormHole()) allWormInputCode();
	myCode += mainLoopInit;
#ifndef PTOLEMY04
	compileRun((SDFScheduler*) scheduler());
#endif
#ifdef PTOLEMY04
        scheduler()->compileRun();
#endif
        if (inWormHole()) allWormOutputCode();
        endIteration(iterations,0);
	myCode += mainLoopTerm;

	// After all stars have run, define a procedure called tkSetup()
	// that contains tkSetup code, if any
	procedures += "void tkSetup() {\n";
	procedures += tkSetup;
	procedures += "}\n";
}


static CGCTclTkTarget targ("TclTk_Target","CGCStar",
"Target for Tcl/Tk C code generation");

static KnownTarget entry(targ,"TclTk_Target");

