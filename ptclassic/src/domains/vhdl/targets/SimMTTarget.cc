static const char file_id[] = "SimMTTarget.cc";
/******************************************************************
Version identification:
@(#)SimMTTarget.cc	1.8 05/30/96

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

 Programmer: William Tsu, Michael C. Williamson

 Target for VHDL code generation and handoff to Synopsys simulator.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SimMTTarget.h"
#include "KnownTarget.h"
#include "paths.h"

// Constructor.
SimMTTarget :: SimMTTarget(const char* name,const char* starclass,
			 const char* desc) :
VHDLTarget(name,starclass,desc) {
  addState(analyze.setState("analyze",this,"YES",
			    "switch for analyzing code."));
  addState(startup.setState("startup",this,"YES",
			    "switch for starting up simulator."));
  addState(simulate.setState("simulate",this,"YES",
			    "switch for simulating."));
  addState(report.setState("report",this,"YES",
			    "switch for generating reports."));
  addState(interactive.setState("interactive",this,"NO",
			    "switch for simulating interactively."));

  // Set the default to not display the code.
  displayFlag.setInitValue("NO");
}

// Clone the Target.
Block* SimMTTarget :: makeNew() const {
  LOG_NEW; return new SimMTTarget(name(), starType(), descriptor());
}

static SimMTTarget proto("SimMT-VHDL", "VHDLStar",
			 "VHDL code generation target for MTI simulation");

static KnownTarget entry(proto,"SimMT-VHDL");

// Write the code to a file.
void SimMTTarget :: writeCode() {
  writeFile(myCode,".vhdl",displayFlag);
}

// Write the command log to a file.
int SimMTTarget :: compileCode() {
  if (int(analyze)) {
    // Generate command for MTI simulator.
    StringList command = "";
    if (progNotFound("vlib")) return FALSE;
    if (progNotFound("vcom")) return FALSE;
    command << "vlib work";
    command << " ; ";
    command << "vcom " << filePrefix << ".vhdl";
    StringList error = "";
    error << "Could not compile " << filePrefix << ".vhdl";
    if (systemCall(command, error, targetHost)) return FALSE;
  }
  // Return TRUE indicating success.
  return TRUE;
}

// Run the code.
int SimMTTarget :: runCode() {
  char 		name[30];
  char 		*p = name;
  strcpy(name, filePrefix);  
  while (*p != 0) {
    if ((*p) >= 'A' && (*p) <= 'Z') {
      *p = *p - 'A' + 'a';
    }
    p++;
  }
  
  // MTI 
  if (int(startup)) {
    // Generate the command to startup the VHDL simulator here - MTI
    StringList command = "";
    StringList sysCommand = "";   
    if (int(interactive)) {
      if (progNotFound("vsim")) return FALSE;
      command << "vsim -i " << name << " ; ";
      StringList error = "";
      error << "Could not simulate " << filePrefix << ".vhdl";
      if (systemCall(command, error, targetHost)) return FALSE;
    }
    else {
      StringList comCode = "";
      comCode << "run -all\n";
      comCode << "quit -f\n";
      writeFile(comCode, ".do", 0);
      if (progNotFound("vsim")) return FALSE;
      command << "vsim " << name << " < " << filePrefix << ".do; ";
      StringList error = "";
      error << "Could not simulate " << filePrefix << ".vhdl";
      if (systemCall(command, error, targetHost)) return FALSE;
    }
    sysCommand << "cd " << (const char*) destDirectory;
    sysCommand << " ; ";
    sysCommand << sysWrapup;
    (void) system(sysCommand);
  }
  return TRUE;
}

ISA_FUNC(SimMTTarget,VHDLTarget);
