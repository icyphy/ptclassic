static const char file_id[] = "SimMTTarget.cc";
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

 Target for VHDL code generation and handoff to Synopsys simulator.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SimMTTarget.h"
#include "KnownTarget.h"
#include <ostream.h>

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
}

// Clone the Target.
Block* SimMTTarget :: makeNew() const {
  LOG_NEW; return new SimMTTarget(name(), starType(), descriptor());
}

static SimMTTarget proto("SimMT-VHDL", "CGStar",
			 "VHDL code generation target for MTI simulation");

static KnownTarget entry(proto,"SimMT-VHDL");

// Write the code to a file.
void SimMTTarget :: writeCode() {
  writeFile(myCode,".vhdl",displayFlag);
/*
  // Change all integers to 4-bit types to simplify simulation.
  StringList command = "";
  command << "cd " << (const char*) destDirectory;
  command << " ; ";
  command << "rm -f " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "sed s/\"REAL\"/\"INTEGER\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  command << "sed s/\"INTEGER\"/\"INTEGER range 0 to 15\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  command << "sed s/\"integer\"/\"INTEGER range 0 to 15\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  system(command);
  */
}

// Write the command log to a file.
int SimMTTarget :: compileCode() {
  if (int(analyze)) {
    // Generate the command to analyze the VHDL code here - Synopsys
    /*
    StringList command = "";
    command << "cd " << (const char*) destDirectory;
    command << " ; ";
    command << "gvan " << filePrefix << ".vhdl";
    system(command);
    */
    
    // Generate command for MTI simulator.
    StringList command = "";
    cout << "Entering SimMTTarget :: compileCode()" << endl << endl;
    command << "cd " << (const char*) destDirectory;
    command << " ; ";
    command << "vlib work; ";
    command << "vcom " << filePrefix << ".vhdl";
    cout << command;
    system(command);
  }
  // Return TRUE indicating success.
  return TRUE;
}

// Run the code.
int SimMTTarget :: runCode() {
  // Synopsis stuff
  /*
  if (int(startup)) {
    // Generate the command to startup the VHDL simulator here - Synopsis
    StringList command = "";
    StringList sysCommand = "";   
    if (int(interactive)) {
      command << "cd " << (const char*) destDirectory;
      command << " ; ";
      command << "vhdldbx " << filePrefix;
      system(command);
    }
    else {
      StringList comCode = "";
      comCode << "run\n";
      comCode << "quit\n";
      writeFile(comCode, ".com", 0);
      command << "cd " << (const char*) destDirectory;
      command << " ; ";
      command << "vhdlsim -i " << filePrefix << ".com " << filePrefix;
      system(command);
    }
    sysCommand << "cd " << (const char*) destDirectory;
    sysCommand << " ; ";
    sysCommand << sysWrapup;
    system(sysCommand);
  }
  */
  // Return TRUE indicating success.

  char 		name[30];
  char 		*p = name;
  strcpy(name, filePrefix);  
  cout << endl << "File prefix is: " << name << endl;
  while (*p != 0) {
    if ((*p) >= 'A' && (*p) <= 'Z') {
      *p = *p - 'A' + 'a';
    }
    cout << *p << " ";
    p++;
  }
  cout << endl;
  cout << "File prefix (converted) is: " << name << endl;
  
  // MTI 
  if (int(startup)) {
    // Generate the command to startup the VHDL simulator here - MTI
    StringList command = "";
    StringList sysCommand = "";   
    if (int(interactive)) {
      cout    << "\nInteractive mode:\n\n";
      cout.flush();
      command << "cd " << (const char*) destDirectory;
      cout    << "cd " << (const char*) destDirectory;
      cout.flush();
      command << " ; ";
      cout    << " ; ";
      cout.flush();
      command << "vsim -i " << name << " ; ";
      cout    << "vsim -i " << name << " ; ";
      cout.flush();
//      cout << "vsim -fn lucidasanstypewriter-12 " << name << " ; ";
      system(command);
    }
    else {
      StringList comCode = "";
      comCode << "run -all\n";
      comCode << "quit -f\n";
      writeFile(comCode, ".do", 0);
      command << "cd " << (const char*) destDirectory;
      cout    << "cd " << (const char*) destDirectory;
      cout.flush();
      command << " ; ";
      cout    << " ; ";
      cout.flush();
      command << "vsim " << name << " < " << filePrefix << ".do; ";
      cout    << "vsim " << name << " < " << filePrefix << ".do; ";
      cout.flush();
      system(command);
    }
    sysCommand << "cd " << (const char*) destDirectory;
    cout       << "cd " << (const char*) destDirectory;
    cout.flush();
    sysCommand << " ; ";
    cout       << " ; ";
    cout.flush();
    sysCommand << sysWrapup;
    system(sysCommand);
  }
  return TRUE;
}

ISA_FUNC(SimMTTarget,VHDLTarget);
