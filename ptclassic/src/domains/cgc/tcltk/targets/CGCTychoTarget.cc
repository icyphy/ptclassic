static const char file_id[] = "CGCTychoTarget.cc";
/******************************************************************
Version identification:
$Id$

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

 Programmer: Sunil Bhave

 Target for generating Tycho based Custom Control panels for CGC

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCTychoTarget.h"
#include "CGCStar.h"
#include "KnownTarget.h"

// constructor
CGCTychoTarget::CGCTychoTarget(const char* name,const char* starclass,
                   const char* desc) : CGCMakefileTarget(name,starclass,desc) {
	funcName.setInitValue("go");
	//StringList compOpts = "$(TCLTK_CFLAGS)";

	// There is no point in including X11 directories here,
	// since they will be different for each platform.
	//compileOptions.setInitValue(hashstring(compOpts));

	//StringList linkOpts = "$(TCLTK_LOADLIBES)";
	//linkOptions.setInitValue(hashstring(linkOpts));

	// Set the default skeleton makefile
	StringList skelMakefile = "$PTOLEMY/lib/cgc/TclTk_Target.mk";
	skeletonMakefile.setInitValue(hashstring(skelMakefile));

	loopingLevel.setInitValue("1");
	addStream("mainLoopInit", &mainLoopInit);
	addStream("mainLoopTerm", &mainLoopTerm);
	addStream("tkSetup", &tkSetup);

	// Add the string state for the Tcl start command
	addState(startCommand.setState("startCommand",this,"makeRunWindow",
        "set the Tcl comment to execute to start the interactive interface")
		 );
}

void CGCTychoTarget :: initCodeStrings() {
	CGCTarget::initCodeStrings();
	mainLoopInit.initialize();
	mainLoopTerm.initialize();
	tkSetup.initialize();
}

// clone
Block* CGCTychoTarget :: makeNew () const {
	LOG_NEW; return new CGCTychoTarget(name(),starType(),descriptor());
}

/////////////////////////////////////////
// codeGenInit
/////////////////////////////////////////

int CGCTychoTarget :: codeGenInit() {
    // Define variables needed by the tcl init file.
    globalDecls << "char *name = \"" << galaxy()->name() << "\";\n"
		<< "int numIterations = "
		<< int(scheduler()->getStopTime()) << ";\n"
		<< "static char startCmd[] = \"" << startCommand << "\";\n"
      //dont include tkMain.c "#include \"tkMain.c\"\n\n";

    // If the system is paused, wait until Go is hit again
    mainLoopInit << "if ( getPollFlag() ) processFlags();\n"
		 << "while (runFlag == -1) Tk_DoOneEvent(0);\n"
		 << "if (runFlag == 0) break;\n";

    mainLoopTerm << "runFlag = 0;\n";

    return CGCTarget::codeGenInit();
}

void CGCTychoTarget :: beginIteration(int repetitions, int depth) {
    CGCTarget::beginIteration(repetitions, depth);
    // Note, unlike SimControl, the following does not support
    // threaded computation
    *defaultStream <<  "if ( getPollFlag() ) processFlags();\n";
}

CodeStream CGCTychoTarget::mainLoopBody() {
    CodeStream body;
    defaultStream = &body;

    // Need specialized code instead of the beginIterations method
    // so that the value can be changed each time the function is run.
    // and infinite iterations can be supported
    StringList iterator = symbol("sdfIterationCounter");
    body << "{\n\tint " << iterator<< ";\n"
	 << iterator << " = 0;\n"
	 << "while (" << iterator << "++ != numIterations) {\n"
	 << mainLoopInit << mainLoop << "}} /* MAIN LOOP END */\n"
	 << mainLoopTerm;

    defaultStream = &myCode;
    return body;
}

void CGCTychoTarget :: frameCode() {
    procedures << "void tkSetup() {\n" << tkSetup << "}\n";
    CGCTarget::frameCode();
}

static CGCTychoTarget targ("Tycho_Target", "CGCStar",
			   "Target for Tcl/Tk C code generation");

static KnownTarget entry(targ, "Tycho_Target");

