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
#include "miscFuncs.h"

// These files included for use within compileCode() and runCode().
#include "ptk.h"
#include "PTcl.h"
#include "CGUtilities.h"

// constructor
CGCTychoTarget::CGCTychoTarget(const char* name,const char* starclass,
                   const char* desc) : CGCTarget(name,starclass,desc) {

	loopingLevel.setInitValue("1");
	addStream("mainLoopTerm", &mainLoopTerm);
        addStream("tychoSetup",&tychoSetup);
        addStream("tkSetup",&tkSetup);
        addState(scriptFile.setState("scriptFile",this,"Enter the Custom Script path here"));
}

void CGCTychoTarget :: initCodeStrings() {
	CGCTarget::initCodeStrings();
	mainLoopTerm.initialize();
        tychoSetup.initialize();
        tkSetup.initialize();

}

// clone
Block* CGCTychoTarget :: makeNew () const {
	LOG_NEW; return new CGCTychoTarget(name(),starType(),descriptor());
}

int CGCTychoTarget :: compileCode() {
  // Create a string and call Tycho to evaluate it
  StringList cmd, error;
  cmd << "puts " << filePrefix << "\n";
  cmd << "::tycho::compileC "
      << expandPathName(destDirectory) << "/" << filePrefix << ".c"
      << " -autostart 1 -autodestroy 1 -modal 1";

  Tcl_Eval( PTcl::activeInterp, cmd.chars() );

  // Always return 1 -- what else can we do?
  return 1;
}

// Run the code. Start another Tycho process and give it the
// "loader" file as argument.
int CGCTychoTarget :: runCode() {
  StringList ldr, fname;
  fname << filePrefix << "_ld.tcl";
  ldr << "::tycho::File::openContext " 
      << expandPathName(destDirectory) << "/" << filePrefix << ".so";
  rcpWriteFile(targetHost, destDirectory, fname, ldr);

  // Exec another Tycho -- note "&" in the command so
  // pigi doesn't go dead...
  StringList cmd, error;
  cmd << "tycho -e " << filePrefix << "_ld.tcl &";
  error << "Could not run " << filePrefix;
  return (systemCall(cmd, error, targetHost) == 0);
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
    
    // If the string is NULL, don't parse through it
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
            if ( *current == '/' && *(current+1) == '*' ) {
                // If this is a comment, go to the end of it
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
            } else if ( ! strncmp(start, "#", 1) ) {
                // In a preprocessor line. Scan to end of line
                while ( *current != '\0' && *current != '\n' ) {
                    length++;
                    current++;
                }
            } else {
                /// Otherwise scan to the end of the statement
                while ( *current != '\0' && *current != ';' ) {
                    length++;
                    current++;
                }
            }
            length++;
	    if ( *current == '*' && *(current+1) == '/' ) {
	      /* skip the 2 symbols */
	      length+=3;
	      current+=3;
	      result << "\n";
	    } else if ( ! strncmp(start, "typedef", 7)) {
	      result << "\n";
	    } else if ( ! strncmp(start, "#", 1)) {
	      result << "\n";
	    } else if ( *current == '\0' ) {
	      result << start;
	      break;
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

void CGCTychoTarget :: frameCode() {
  char* tycgcpath = expandPathName("$PTOLEMY/tycho/src/tycgc/tycgc.h");

  // Add the comment, and include files
  myCode << headerComment() << "\n\n"
  << include
  << "#include \""
  << tycgcpath
  << "\"\n\n";

    // Add static variables used by all modules
  myCode << comment("Name of this CGC module")
    << "static char *moduleName = \"" 
    << galaxy()->name() << "\";\n"
    << comment("The name of the script file that creates the user interface")
    << "static char *scriptFile = \""
    << (const char *)scriptFile << "\";\n\n"
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

  // Add tychoSetup to the procedures string
  procedures << "static void tychoSetup() {\n" << tychoSetup << "}\n";
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
   
  // Mess about to get the right name for the initialization function
  char buffer[120];
  char *b = buffer;
  strcpy(buffer,galaxy()->name());
  *b++ = toupper(*b);
  while ( *b != '\0' ) {
    *b++ = tolower(*b);
  }

  StringList initProcedure = "int ";
  initProcedure << buffer << "_Init"
    <<"(Tcl_Interp *interp) {\n"
        << "/* Adds a single interface command */\n"
        <<"Tcl_CreateCommand(interp, "
	<<"\""
	<< galaxy()->name()
	<<"\", tclinterface, (ClientData) NULL, (Tcl_CmdDeleteProc *) NULL);\n"
	<<"return TCL_OK;\n}\n";

  char* tclifpath = expandPathName("$PTOLEMY/tycho/src/tycgc/tclif.c");

  myCode << setupDecl << wrapupDecl << executeDecl 
  << "#include \""
  << tclifpath
  << "\"\n"
  << initProcedure;

  // after generating code, initialize code strings again.
  initCodeStrings();
}

static CGCTychoTarget targ("Tycho_Target", "CGCStar",
			   "Target for Tcl/Tk C code generation with Custom Control Panels");

static KnownTarget entry(targ, "Tycho_Target");







