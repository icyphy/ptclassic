// This may look like C code, but it is really -*- C++ -*-
static const char file_id[] = "CGCMultiTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) %Q% The Regents of the University of California.
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

 Programmers: Christopher Hylands, Jose Pino

 This is a Makefile target class for CGCdomain.

*******************************************************************/

/* This target is used primarily by the unixMulti_C target.  The
 * problem is that unixMulti_C uses inet_addr().  Under Solaris2.x
 * inet_addr() is in libsocket, and libsocket requires libnsl.  So to
 * compile unixMulti_C target files, we need to add -lsocket -lnsl to
 * the link line.
 *
 * unixMulti_C does uses default_C to actually generate code, and
 * unixMulti_C does not pass args down to default_C, in part because
 * we have no way of know what architecture the compiles are actually
 * happening on.  A good test of unixMulti_C would be to compile on a
 * SunOS machine, a DEC Alpha and a Solaris machine.
 * 
 * makefile_C generates a small makefile that is rcp'd over to the
 * remote machine.  The generated makefile uses
 * $PTOLEMY/lib/cgc/makefile_C.mk as a starting point, and then
 * appends lines to it.  The user may modify makefile_C.mk and add
 * site dependent rules and variables there.  If the user wants to
 * have site dependent include files on the remote machines, then they
 * could add 'include $(ROOT)/mk/mysite.mk' to makefile_C.mk, and that
 * file would be included on the remote machines at compile time.
 * 
 * One drawback of appending to makefile_C.mk is that the first rule
 * is what gets run if you run 'make' without a rule or target name.
 * However, we rsh to the remote machine and do a 'make all', so the
 * user can modify makefile_C.mk add add other rules to it.

 * The generated makefile is named after the universe.  If the
 * universe is called bigBang, then the makefile will be called
 * bigBang.mk.  We name the generated makefiles so that more than one
 * makefile can exist in the users' directory.
 * 
 * On the remote machine, we assume:
 *	1) $PTOLEMY and $PTARCH are set on the remote machine when rshing
 *	2) $PTOLEMY/mk/config-$PTARCH.mk and any makefile files included by
 *	that file are present.
 *	3) a make binary is present. (not necessarily GNU make)
 *
 * If the remote machine does not fulfill these constraints, then the
 * user should use default_C.
 *
 * makefile_C does not assume GNU make, so in the default situation
 * we could not include mk/common.mk.  The reason not to assume GNU make
 * is that we are not sure what the user's path is like when they log
 * in.  However, makefile_C has a target parameter 'skeletonMakefile' that
 * allows the user to specify an arbitrary makefile that could or
 * could need gmake.  If the 'skeletonMakefile' target parameter is empty,
 * then we use makefile_C.mk
 *
 * The target parameter 'appendToMakefile' controls whether we append rules
 * to the makefile or just copy it over to the remote machine.  In the
 * default situation, appendToMakefile is true and we append our rules
 * to makefile_C.mk
 *  
 */
#ifdef __GNUG__
#pragma implementation
#endif

#include "pt_fstream.h"
#include <stdio.h>		// Need BUFSIZ
#include "CodeBlock.h"
#include "CGUtilities.h"
#include "CGCStar.h"
#include "KnownTarget.h"
#include "CGCMakefileTarget.h"
#include "CGCTarget.h"


CGCMakefileTarget::CGCMakefileTarget(const char* name,const char* starclass,
				     const char* desc)
: CGCTarget(name,starclass,desc)
{
				// Make compileCommand invisible and unsettable
  compileCommand.clearAttributes(A_SETTABLE);

				// Specify makefile to be used.
  addState(skeletonMakefile.setState("skeletonMakefile",this,
				     "$PTOLEMY/lib/cgc/makefile_C.mk",
  "makefile to use as a base for copying to remote machine"));

				// Determine whether we append to the
				// Makefile or not.
  addState(appendToMakefile.setState("appendToMakefile",this,"TRUE",
  "determine whether we append to the skeletonMakefile, or just copy it over")
	   );

}

CGCMakefileTarget :: ~CGCMakefileTarget() {
}

Block* CGCMakefileTarget::makeNew() const {
    LOG_NEW; return new CGCMakefileTarget(name(),starType(),descriptor());
}

void CGCMakefileTarget :: writeCode()
{
    pt_ifstream input;
    char inBuf[BUFSIZ];

				// Generate the C code.
    writeFile(myCode,".c",displayFlag);
    if (!onHostMachine(targetHost)) {
	const char* header = "$PTOLEMY/src/domains/cgc/rtlib/CGCrtlib.h";
	const char* source = "$PTOLEMY/src/domains/cgc/rtlib/CGCrtlib.c";
	rcpCopyFile(targetHost, destDirectory, header);
	rcpCopyFile(targetHost, destDirectory, source);
    }
				// Copy the makefile
    StringList generatedMakefile;
    StringList makefileName;
    makefileName << filePrefix << ".mk";

    if ( appendToMakefile ) {
				// Append append rules to the makefile
      
				// Copy the makefile to our generated makefile
      // FIXME: is there a better way to read in a file and copy it to
      // a stream?

      input.open(skeletonMakefile);
      while ( input.good() && !input.eof() ) {
	input.getline(inBuf, BUFSIZ);
	generatedMakefile << inBuf << "\n";
      }
      input.close();

      /* Code duplication from CreateSDFStar - FIXME */
      if (compileOptionsStream.numPieces()) {
	  char* expandedCompileOptionsStream =
	      expandPathName(compileOptionsStream);
	  generatedMakefile << "OTHERCFLAGS= ";
	  generatedMakefile << expandedCompileOptionsStream << ' ';
	  delete [] expandedCompileOptionsStream;
      }
      if(strlen((const char*) compileOptions) > 0) {
				// If we have not printed OTHERCFLAGS yet,
				// then do it now
	  if (!compileOptionsStream.numPieces())
	     generatedMakefile << "OTHERCFLAGS= ";
	  generatedMakefile << (const char *)compileOptions;
      }
      generatedMakefile << "\n";


				// The GNU make info page says:
				// "`N' is made automatically from
      				//  `N.o' by running the linker 
      				//  (usually called `ld') via the C
      				//  compiler. The precise command
      				//  used is
      				//  `$(CC) $(LDFLAGS) N.o $(LOADLIBES)'."
      /* Code duplication from CreateSDFStar - FIXME */
      if (linkOptionsStream.numPieces()) {
	  char* expandedLinkOptionsStream = expandPathName(linkOptionsStream);
	  generatedMakefile << "LOADLIBES= ";
	  generatedMakefile << expandedLinkOptionsStream << ' ';
	  delete [] expandedLinkOptionsStream;
      }     
      if(strlen((const char*) linkOptions) > 0) {
	  if (!linkOptionsStream.numPieces())
				// If we have not printed LOADLIBES= yet,
				// then do it now.
	     generatedMakefile << "LOADLIBES= ";
	  generatedMakefile << (const char *)linkOptions;
      }
      generatedMakefile << "\n";
      
				// Append rules to the end of the makefile
      generatedMakefile << "all: " << (const char *) filePrefix << "\n"
	       << filePrefix << ": " << filePrefix << ".o\n";
      writeFile(generatedMakefile, ".mk");
      rcpWriteFile(targetHost, destDirectory, makefileName, generatedMakefile);
    } else {
				// We are using a user provided makefile.
				// Just copy it over.
      rcpCopyFile(targetHost, destDirectory, skeletonMakefile, 1,
		  makefileName); 
    }
}

int CGCMakefileTarget :: compileCode()
{
    // Run make remotely
    StringList cmd, error;

    cmd << "make -f " << filePrefix << ".mk all";
    error << "Could not run ` " << cmd << "'";
    return (systemCall(cmd, error, targetHost) == 0);
}

ISA_FUNC(CGCMakefileTarget,CGCTarget);

static CGCMakefileTarget targ("Makefile_C","CGCStar",
"A target for C code generation using Ptolemy makefiles");

static KnownTarget entry(targ,"Makefile_C");

