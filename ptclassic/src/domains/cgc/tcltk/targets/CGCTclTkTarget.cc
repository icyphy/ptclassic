static const char file_id[] = "CGCTclTkTarget.cc";
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

 Programmer: E. A. Lee

 Base target for C code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCTclTkTarget.h"
#include "CGCStar.h"
#include "KnownTarget.h"

// constructor
CGCTclTkTarget::CGCTclTkTarget(const char* name,const char* starclass,
                   const char* desc) : CGCTarget(name,starclass,desc) {
	funcName.setInitValue("go");
	StringList compOpts =
		  "-I$PTOLEMY/tcltk/tk/include -I$PTOLEMY/tcltk/tcl/include "
		  "-I$PTOLEMY/src/domains/cgc/tcltk/lib "
		  "-I$PTOLEMY/src/ptklib";
	// There is no point in including X11 directories here,
	// since they will be different for each platform.
	compileOptions.setInitValue(hashstring(compOpts));

	// Note that as a last resort, a guess at the X11 library is given
	StringList linkOpts =
		  "-L$PTOLEMY/tcltk/tk.$ARCH/lib "
		  "-L$PTOLEMY/tcltk/tcl.$ARCH/lib "
		  "-L$PTOLEMY/lib.$ARCH "
		  "-L$PTOLEMY/tcltk/tk.$ARCH/lib "
		  "-L/usr/X11/lib "
		  "-ltk -ltcl -lptk -lX11 -lsocket -lnsl -lm";
	linkOptions.setInitValue(hashstring(linkOpts));
	loopingLevel.setInitValue("1");
	addStream("mainLoopInit", &mainLoopInit);
	addStream("mainLoopTerm", &mainLoopTerm);
	addStream("tkSetup", &tkSetup);
}

// Modify the compile line
StringList CGCTclTkTarget :: compileLine(const char* fName) {
    // If the appropriate environment variable is defined,	
    // include the location of the X11 directories.
    char *x11dirstring = getenv("PTX11DIR");
    if (x11dirstring) {
	compileOptionsStream << " -I" << x11dirstring << "/include ";
	linkOptionsStream << " -L" << x11dirstring << "/lib ";
    }
    return CGCTarget::compileLine(fName);
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
    globalDecls << "char *name = \"" << galaxy()->name() << "\";\n"
		<< "int numIterations = "
		<< (int)scheduler()->getStopTime() << ";\n"
		<< "#include \"tkMain.c\"\n\n";

    // If the system is paused, wait until Go is hit again
    mainLoopInit << "if ( getPollFlag() ) processFlags();\n"
		 << "while (runFlag == -1) Tk_DoOneEvent(0);\n"
		 << "if (runFlag == 0) break;\n";

    mainLoopTerm << "runFlag = 0;\n";

    return CGCTarget::codeGenInit();
}

void CGCTclTkTarget :: beginIteration(int repetitions, int depth) {
    CGCTarget::beginIteration(repetitions, depth);
    // Note, unlike SimControl, the following does not support
    // threaded computation
    *defaultStream <<  "if ( getPollFlag() ) processFlags();\n";
}

CodeStream CGCTclTkTarget::mainLoopBody() {
    CodeStream body;
    defaultStream = &body;

    // Need specialized code instead of the beginIterations method
    // so that the value can be changed each time the function is run.
    // and infinite iterations can be supported
    StringList iterator= symbol("sdfIterationCounter");
    body << "{\n\tint " << iterator<< ";\n"
	 << iterator << "=0;\n"
	 << "while (" << iterator << "++ != numIterations) {\n"
	 << mainLoopInit << mainLoop << "}} /* MAIN LOOP END */\n"
	 <<  mainLoopTerm;

    defaultStream = &myCode;
    return body;
}

void CGCTclTkTarget :: frameCode() {
    procedures << "void tkSetup() {\n" << tkSetup << "}\n";
    CGCTarget::frameCode();
}

static CGCTclTkTarget targ("TclTk_Target","CGCStar",
"Target for Tcl/Tk C code generation");

static KnownTarget entry(targ,"TclTk_Target");

