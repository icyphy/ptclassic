static const char file_id[] = "CGCTychoTarget.cc";
/******************************************************************
Version identification:
$Id$

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

 Target for generating Tycho based Custom Control panels for CGC

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <string.h>
#include <ctype.h>
#include "CGCTychoTarget.h"
#include "CGCStar.h"
#include "KnownTarget.h"

// constructor
CGCTychoTarget::CGCTychoTarget(const char* name,const char* starclass,
                   const char* desc) : CGCTarget(name,starclass,desc) {

	addState(skeletonMakefile.setState("skeletonMakefile",this,
					   "$PTOLEMY/lib/cgc/TclTk_Target.mk",
					   "makefile to use as a base for copying to remote machine"));

	loopingLevel.setInitValue("1");
	addStream("mainLoopTerm", &mainLoopTerm);

}

void CGCTychoTarget :: initCodeStrings() {
	CGCTarget::initCodeStrings();
	mainLoopTerm.initialize();
}

// clone
Block* CGCTychoTarget :: makeNew () const {
	LOG_NEW; return new CGCTychoTarget(name(),starType(),descriptor());
}

/////////////////////////////////////////
// codeGenInit
/////////////////////////////////////////

int CGCTychoTarget :: codeGenInit() {

    mainLoopTerm << comment("If timer has expired, return")
      << "if ( Ty_TimerElapsed() ) {\n"
      << " return 1;\n}\n";

    return CGCTarget::codeGenInit();
}

CodeStream CGCTychoTarget::mainLoopBody() {
    CodeStream body;
    defaultStream = &body;

    body << "while (iterationCounter != numIterations) {\n"
      << mainLoop << "/* Main Loop End */\n"
      << mainLoopTerm
      << "iterationCounter++;\n}\n";
    defaultStream = &myCode;
    return body;
}

void CGCTychoTarget :: addStaticDecls
                 ( StringList &result, const char *string ) {
  int length;
  char *start;
  char *current = (char *)string;

  // If the string is NULL, dont parse through it
  if (!(string == NULL)) {
  while ( *current != '\0' ) {
    // skip white space
    while ( *current != '\0' 
	    && ( *current == ' ' || *current == '\n' || *current == '\t' )) {
      current++;
    }
    if ( *current == '\0' ) {
      result << "\n";
      break;
    }
    length = 0;
    start = current;
    // If this is a comment, go to the end of it
    if ( *current == '/' && *(current+1) == '*' ) {
      current += 2;
      while ( *current != '\0'
	      && *current != '*' && *(current+1) != '/') {
	current++;
	length++;
      }

    } else if ( ! strncmp(start, "typedef", 7) ) {
      // In a typedef. This is fragile and should match braces
      while ( *current != '\0' && *current != '}' ) {
	length++;
	current++;
      }
      while ( *current != '\0' && *current != ';' ) {
	length++;
	current++;
      }
    } else {
      while ( *current != '\0' && *current != ';' ) {
	length++;
	current++;
      }
    }
    length++;
    if ( *current == '\0' ) {
      result << "static" << start;
      break;
    } else {
      if ( *current == '*' && *(current+1) == '/' ) {
	/* skip the 2 symbols */
	length+=3;
	current+=3;
	result << "\n";
      } else if ( ! strncmp(start, "typedef", 7)) {
	result << "\n";
      } else {
	result << "\n" << "static ";
      }
      char *temp = new char[length+1];
      temp[length] = '\0';
      strncpy(temp,start,length);
      result << temp;
      delete [] temp; 
      current++;
    }
  }
  }
}

void CGCTychoTarget :: frameCode() {
  // This frameCode is lifted from the CGCTarget

  // Add the comment, and include files
  myCode << headerComment() << "\n\n"
	 << include
	 << "#include \"tycgc.h\"\n"
	 << "#include \"tytimer.h\"\n\n";

    // Add static variables used by all modules
  myCode << comment("Name of this CGC module")
    << "static char *moduleName = \"" 
    << galaxy()->name() << "\";\n\n"
    << comment("Loop counters")
      << "int numIterations = "
      << int(scheduler()->getStopTime()) << ";\n"
      << "int iterationCounter;\n"
      << "static int sdfLoopCounter;\n\n"
      << comment("Tcl interface data")
	<< "static char tclcountername[40];\n"
	<< "static char updatetclcounter;\n\n";

  // Add static variable decls from stars, with
  // a "static" in front of each
  addStaticDecls(myCode, (const char *)globalDecls);
  addStaticDecls(myCode, (const char *)mainDecls);


  // Add the procedures
  myCode << procedures;

  StringList setupDecl = "\nstatic void setup() {\n";
  StringList wrapupDecl = "\nstatic void wrapup() {\n";
  StringList executeDecl = "\nstatic int execute() {\n";

  setupDecl << "sdfLoopCounter = 0;\n"
	    << "iterationCounter = 0;\n"
	    << commInit << mainInit 
	    << "}\n";
  wrapupDecl << mainClose << "\n}\n";

  executeDecl << mainLoopBody()
    << comment("If we go to here, the iteration counter was reached")
      << comment("so call wrapup and return 0")
	<< "wrapup();\n"
	<< "return 0;\n}\n";
    
  char buffer[120];
  strcpy(buffer,galaxy()->name());
  buffer[0] = toupper(buffer[0]);


  StringList initProcedure = "static int ";
  initProcedure << buffer << "_Init"
    <<"(Tcl_Interp *interp) {
      /* Adds a single interface command */\n"
        <<"Tcl_CreateCommand(interp, "
	<<"\""
	<< galaxy()->name()
	<<"\", tclinterface, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);\n"
	<<"return TCL_OK;\n}\n";

  myCode << setupDecl << wrapupDecl << executeDecl 
	 << "#include \"tycgcif.c\"\n\n"
	 << initProcedure;

  // after generating code, initialize code strings again.
  initCodeStrings();
}

static CGCTychoTarget targ("Tycho_Target", "CGCStar",
			   "Target for Tcl/Tk C code generation with Custom Control Panels");

static KnownTarget entry(targ, "Tycho_Target");







