static const char file_id[] = "CGCVISSimTarget.cc";
/******************************************************************
Version identification:
@(#)CGCVISTarget.cc	1.4 06/29/96
 
@Copyright (c) 1995-%Q% The Regents of the University of California.
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
 
 Programmer: William Chen
 
*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCStar.h"
#include "CGCTarget.h"
#include "CGCVISSimTarget.h"
#include "Error.h"
#include "KnownTarget.h"
#include "CGUtilities.h"
// ----------------------------------------------------------------------------

CGCVISSimTarget::CGCVISSimTarget(const char* name,const char* starclass,const char* desc, const char* assocDomain):CGCTarget(name,starclass,desc,assocDomain) {
  addState(linkCommand.setState("linkCommand",this,"ld",
				   "command for linking code."));
  addState(simulate.setState("simulate",this,"YES",
			    "switch for simulating."));
  addStream("remoteObjFiles", &remoteObjFilesStream);

  functionCounter = 0;

  compileCommand.setState("compileCommand",this,"cc","command for compiling code.");

  compileOptions.setState("compileOptions",this,"-c -DINCAS -fast -xO4 -xdepend -xchip=ultra -xarch=v8plusa -V -I${VSDKHOME}/include ${VSDKHOME}/util/vis.il","options to be specified for compiler.");

  linkOptions.setState("linkOptions",this,"-dn -M ${INCASHOME}/util/prom.ld ${INCASHOME}/util/traps.o ${INCASHOME}/util/incas_utils.o /usr/lib/libc.a","options to be specified for linker.");

}

// ----------------------------------------------------------------------------

Block* CGCVISSimTarget :: makeNew() const {
	LOG_NEW; return new CGCVISSimTarget(name(),starType(),descriptor());
}

// ----------------------------------------------------------------------------

void CGCVISSimTarget :: genProfile(int funcid){
        *defaultStream << "sim_break" << funcid << "();\n";
}

void CGCVISSimTarget :: writeFiring(Star& s,int) { // depth parameter ignored
        genProfile(functionCounter);
	s.run();
	functionCounter++;
}

// ----------------------------------------------------------------------------

// compile the file <filePrefix>.c
int CGCVISSimTarget :: compileCode() {
    // invoke the compiler
    StringList compilecmd, compileerror, file;
    file << filePrefix << ".c ";
    compilecmd << compileLine(file);
    compileerror << "Could not compile " << file;
    return (systemCall(compilecmd, compileerror, targetHost) == 0);
}

// return compile command
StringList CGCVISSimTarget :: compileLine(const char* fName) {
    // Get the compiler arguments with environment
    // variables expanded
    StringList compileArgs = getCompileOptions(TRUE);
    StringList srcFiles = getDependentCFiles(TRUE);
    StringList compilecmd = (const char*) compileCommand;
    if (compileArgs.length() > 0) compilecmd << " " << compileArgs;
    compilecmd << " " << fName;
    if (srcFiles.length() > 0) compilecmd << " " << srcFiles;
    compilecmd << " ";
    return compilecmd;
}

// ----------------------------------------------------------------------------

// write the files
void CGCVISSimTarget :: writeCode() {
    writeFile(myCode, ".c", displayFlag);
    processDependentFiles();
    writeSimFile();
}

void CGCVISSimTarget :: writeSimFile() {
  // Make sure to do the sim file uniquely too!!!
  StringList simCode;
  //simCode << "echo on\n";
  simCode << "focus ieu1\n";
  simCode << "load 0 ram1"<< " " << filePrefix << "\n";
  simCode << "# main\n";
  simCode << "breakpoint add &main\n";
  simCode << "run\n";
  simCode << "wait\n";
  simCode << "time\n";
  simCode << "echo main\n";

  Galaxy& g = *galaxy();
  GalStarIter nextStar(g);
  CGCStar* s;
  int funcindex=0;
  while ((s = (CGCStar*)nextStar++) != 0) {
    simCode << "#" << " " << s->name() << "\n";
    simCode << "breakpoint add &sim_break" << funcindex << "\n";
    for(int i=0;i < s->repetitions.num();i++){
      simCode << "run\n";
      simCode << "wait\n";
    }
    simCode << "wait\n";
    simCode << "time\n";
    simCode << "echo "<< s->name()<< "\n";
    funcindex++;
  }
  simCode << "# exit\n";
  simCode << "run\n";
  simCode << "wait\n";
  simCode << "time\n";
  //simCode << "quit\n";

  writeFile(simCode, ".sim", 0);
}
// ----------------------------------------------------------------------------

// link the file
int CGCVISSimTarget :: loadCode(){
    StringList linkcmd, linkerror, file;
    file << filePrefix << ".o";
    linkcmd << loadLine(file);
    linkerror << "Could not link " << file;
    return (systemCall(linkcmd, linkerror, targetHost) == 0);
}

// return the link command
StringList CGCVISSimTarget :: loadLine(const char* fName){
    StringList linkArgs = getLinkOptions(TRUE);
    StringList objFiles = getDependentObjFiles(TRUE);
    StringList linkcmd = (const char*) linkCommand;
    if (linkArgs.length() > 0) linkcmd << " " << linkArgs;
    linkcmd << " " << fName;
    if (objFiles.length() > 0) linkcmd << " " << objFiles;
    linkcmd << " ";
    linkcmd << "-o" << " " << filePrefix;
    return linkcmd;
}

StringList CGCVISSimTarget :: getDependentObjFiles(int expandEnvironmentVars){
    StringList objectFileList = "";
    objectFileList << remoteObjFilesStream;

    // Conditionally expand environment variables in the compile options
    if (expandEnvironmentVars && objectFileList.length() > 0) {
	char* allObjFiles = expandPathName(objectFileList);
	objectFileList = allObjFiles;
	delete [] allObjFiles;
   }
   return objectFileList;
}

// ----------------------------------------------------------------------------

int CGCVISSimTarget :: runCode() {
  if (int(simulate)) {
    StringList simcmd, simerror;
    simcmd << "xterm -e";
    simcmd << " " << "${INCASHOME}/bin/incas_startup\n";
    simerror << "Could not start simulator";
    return (systemCall(simcmd, simerror, targetHost) == 0);
  }
  return TRUE;
}

// ----------------------------------------------------------------------------

ISA_FUNC(CGCVISSimTarget,CGCTarget);

static CGCVISSimTarget myTargetPrototype("CGCVISSim","CGCStar","A VIS Simulator target to profile VIS code");

static KnownTarget entry(myTargetPrototype,"CGCVISSim");



