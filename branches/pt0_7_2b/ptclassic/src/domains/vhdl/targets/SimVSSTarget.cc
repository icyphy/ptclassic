static const char file_id[] = "SimVSSTarget.cc";
/******************************************************************
Version identification:
@(#)SimVSSTarget.cc	1.55 09/10/99

Copyright (c) 1990-1999 The Regents of the University of California.
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

 Target for VHDL code generation and interaction with Synopsys simulator.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SimVSSTarget.h"
#include "KnownTarget.h"
#include "CGUtilities.h"
#include "paths.h"
#include <stdio.h>              // printf()
#include <unistd.h>             // getpid()


// Constructor.
SimVSSTarget :: SimVSSTarget(const char* name,const char* starclass,
			     const char* desc) :
VHDLTarget(name,starclass,desc) {
  addState(synopsys.setState("$SYNOPSYS",this,"",
			    "value for SYNOPSYS environment variable."));
  // Needed by the "gvan" vhdl code analyzer.
  addState(arch.setState("$ARCH",this,"",
			    "value for ARCH environment variable."));
  // Needed by the "(pt)vhdlsim" vhdl simulator.
  addState(simarch.setState("$SIM_ARCH",this,"",
			    "value for SIM_ARCH environment variable."));
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

  addCodeStreams();
  initCodeStreams();
  
  // Set the default to not display the code.
  displayFlag.setInitValue("NO");

  // Use the process id to uniquely name the sockets.
  int pid = (int) getpid();
  // Because of the naming style we use and because the
  // address name (path name) is limited to 14 characters
  // by struct sockaddr, and because process ids are up to 5 digits,
  // anything more than an additional character in length will
  // fail (e.g., multiplying by 100 instead of 10).

  // But now that we use struct sockaddr_un, we have 108 characters,
  // so scaling up the id value is fine and will prevent applications
  // from colliding with one another (even though this would be rare).
  pairNumber = pid*1000;
}

// Clone the Target.
Block* SimVSSTarget :: makeNew() const {
  LOG_NEW; return new SimVSSTarget(name(), starType(), descriptor());
}

static SimVSSTarget proto("SimVSS-VHDL", "VHDLStar",
			  "VHDL code generation target for Synopsys simulation");
static KnownTarget entry(proto,"SimVSS-VHDL");

void SimVSSTarget :: begin() {
  // Init members here to avoid erasing code and items created
  // during begin method of stars.
  VHDLTarget::setup();

  initVHDLObjLists();
  initCodeStreams();

  VHDLTarget::begin();
}

void SimVSSTarget :: setup() {
  // NOTE: Do not init code streams here because doing so will clobber
  // code generated in the VHDLCSend, VHDLCReceive stars' begin methods.

  synopsys.initialize();
  arch.initialize();
  simarch.initialize();

  // Test the values of parameters SYNOPSYS, ARCH, and SIM_ARCH.
  // For each parameter, if it is set, then set it in the environment.
  // Otherwise, leave the environment unchanged.

  // putenv() requires us to save the string defining the setting:
  // Under SunOS4.1.3, the man page for putenv() says:
  //    string points to a string of the form `name=value'  putenv()
  //    makes  the  value  of the environment variable name equal to
  //    value by altering an existing variable  or  creating  a  new
  //    one.   In  either  case,  the  string  pointed  to by string
  //    becomes part of the environment, so altering the string will
  //    change  the  environment.   The  space  used by string is no
  //    longer used once a new string-defining  name  is  passed  to
  //    putenv().

  // char* for test purposes.
  char* testString = 0;

  printf("\n");

  // Set SYNOPSYS in the environment if the target parameter is set.
  StringList synopsysValue = synopsys.currentValue();
  if (synopsysValue.length()) {
      StringList synopsysSetting = "SYNOPSYS=";
      synopsysSetting << synopsysValue;
      char* saveSynopsysString = savestring(synopsysSetting);
      putenv(saveSynopsysString);
  }

  // Report the value of SYNOPSYS in the environment.
  testString = getenv("SYNOPSYS");
  if (testString) {
      printf("SYNOPSYS=%s\n", testString);
  }
  else {
      printf("SYNOPSYS is unset\n");
  }

  // Set ARCH in the environment if the target parameter is set.
  StringList archValue = arch.currentValue();
  if (archValue.length()) {
      StringList archSetting = "ARCH=";
      archSetting << archValue;
      char* saveArchString = savestring(archSetting);
      putenv(saveArchString);
  }

  // Report the value of ARCH in the environment.
  testString = getenv("ARCH");
  if (testString) {
      printf("ARCH=%s\n", testString);
  }
  else {
      printf("ARCH is unset\n");
  }

  // Set SIM_ARCH in the environment if the target parameter is set.
  StringList simarchValue = simarch.currentValue();
  if (simarchValue.length()) {
      StringList simarchSetting = "SIM_ARCH=";
      simarchSetting << simarchValue;
      char* saveSimarchString = savestring(simarchSetting);
      putenv(saveSimarchString);
  }

  // Report the value of SIM_ARCH in the environment.
  testString = getenv("SIM_ARCH");
  if (testString) {
      printf("SIM_ARCH=%s\n", testString);
  }
  else {
      printf("SIM_ARCH is unset\n");
  }

  // By default, write a command file.
  writeCom = 1;
}

// Routines to construct CG wormholes, using the
// $PTOLEMY/src/domains/cgc/targets/main/CGWormTarget
CommPair SimVSSTarget :: fromCGC(PortHole&) {
  CommPair pair(new CGCVSend,new VHDLCReceive);
  configureCommPair(pair);
  return pair;
}

CommPair SimVSSTarget :: toCGC(PortHole&) {
  CommPair pair(new CGCVReceive,new VHDLCSend);
  configureCommPair(pair);
  return pair;
}

void SimVSSTarget :: configureCommPair(CommPair& pair) {
  pair.cgcStar->setState("destDir", hashstring(destDirectory));
  pair.cgcStar->setState("filePre", hashstring(filePrefix));

  StringList prNum = pairNumber;
  const char* hashPrNum = hashstring(prNum);
  
  pair.cgcStar->setState("pairNumber", hashPrNum);
  pair.cgStar->setState("pairNumber", hashPrNum);
  pairNumber++;
}

// Initial stage of code generation.
void SimVSSTarget :: headerCode() {
}

///////////////////////////////////////////////////////////////////////
//	FIXME:  Integer datatypes are assumed many places
//	throughout this target and in the supporting stars
//	and c-models linked into the simulator.  Need to
//	expand this to floats, etc.
///////////////////////////////////////////////////////////////////////

// Combine all sections of code.
void SimVSSTarget :: frameCode() {
  CodeStream code;
  code << headerComment();

//  StringList galName = galaxy()->name();
  StringList galName = (const char *)filePrefix;
  StringList topName = galName;
  topName << "_top";

  StringList temp_top_uses = "";
  temp_top_uses << top_uses;
  top_uses.initialize();
  top_uses << "-- top-level use clauses\n";
  top_uses << temp_top_uses;
  top_uses << "library SYNOPSYS,IEEE;\n";
  top_uses << "use SYNOPSYS.ATTRIBUTES.all;\n";
  top_uses << "use IEEE.STD_LOGIC_1164.all;\n";
  top_uses << "use std.textio.all;\n";

  top_entity << "-- top-level entity\n";
  top_entity << "entity ";
  top_entity << topName;
  top_entity << " is\n";
  top_entity << "end ";
  top_entity << topName;
  top_entity << ";\n";

  StringList label = galName;
  label << "_proc";
  topCompDeclList.put(label, &mainPortList, &mainGenList,
		      galName, &mainPortList, &mainGenList);

  top_architecture << "-- top-level architecture\n";
  top_architecture << "architecture ";
  top_architecture << "structure";
  top_architecture << " of ";
  top_architecture << topName;
  top_architecture << " is\n";
  top_architecture << addComponentDeclarations(&topCompDeclList);
  top_architecture << "\n";
  top_architecture << addSignalDeclarations(&topSignalList);
  top_architecture << "\n";
  top_architecture << "begin\n";

  top_architecture << addComponentMappings(&topCompDeclList);
  top_architecture << "end ";
  top_architecture << "structure";
  top_architecture << ";\n";

  // Generate the top-level configuration.
  top_configuration << "-- top-level configuration\n";
  top_configuration << "configuration ";
  top_configuration << galName;
  top_configuration << "_parts";
  top_configuration << " of ";
  top_configuration << topName;
  top_configuration << " is\n";
  top_configuration << "for structure\n";
  top_configuration << indent(1) << "for all:" << galName;
  top_configuration << " use entity work." << galName;
  top_configuration << "(behavior); end for;\n";
  top_configuration << cli_configs;
  top_configuration << "end for;\n";
  top_configuration << "end ";
  top_configuration << galName;
  top_configuration << "_parts;\n";

  // Generate the entity_declaration.
  entity_declaration << "-- entity_declaration\n";
  entity_declaration << "entity ";
  entity_declaration << galName;
  entity_declaration << " is\n";
  entity_declaration << addGenericRefs(&mainGenList);
  entity_declaration << addPortRefs(&mainPortList);
  entity_declaration << "end ";
  entity_declaration << galName;
  entity_declaration << ";\n";

  // Generate the architecture_body_opener.
  architecture_body_opener << "-- architecture_body_opener\n";
  architecture_body_opener << "architecture ";
  architecture_body_opener << "behavior";
  architecture_body_opener << " of ";
  architecture_body_opener << galName;
  architecture_body_opener << " is\n";
  architecture_body_opener << "begin\n";

/////////////////////////////////////////////
  
  code << "\n" << top_uses;
  code << "\n" << entity_declaration;
  code << "\n" << architecture_body_opener;
  code << "\n" << "process";
  // Special inserted line for initCode methods to use
  code << "\n" << mainDecls;
  code << "\n" << variable_declarations;
  code << "\n" << "begin";
  code << "\n" << loopOpener;
  
  // myCode contains the main action of the main process
  code << myCode;
  
  code << "\n" << loopCloser;
  code << "\n" << architecture_body_closer;

  code << "\n" << top_uses;
  code << "\n" << top_entity;
  code << "\n" << top_architecture;
  code << "\n" << top_configuration;

  myCode = code;
  
  // Initialize lists.
//  firingVariableList.initialize();
//  variableList.initialize();
}

// Write the code to a file.
void SimVSSTarget :: writeCode() {
  writeFile(myCode,".vhdl",displayFlag);

  // Write a .synopsys_vss.setup file to satisfy gvan and ptvhdlsim
  // so that they can find the PTVHDLSIM library directory.
  StringList setupText = "";
  setupText << "-- Synopsys VSS Setup File" << "\n";
  setupText << "-- DO NOT DELETE" << "\n";
  setupText << "-- Synopsys neds this in order to find" << "\n";
  setupText << "-- the PTVHDLSIM library" << "\n";
  setupText << "" << "\n";
  setupText << "PTVHDLSIM : $PTOLEMY/obj.$PTARCH/utils/ptvhdlsim" << "\n";
  setupText << "\n";

  // Write the file, but do not display it.
  rcpWriteFile(targetHost, destDirectory, ".synopsys_vss.setup",
	       setupText, 0);

  if (writeCom) {
    writeComFile();
  }
  singleUnderscore();
}

// Write the command log to a file.
int SimVSSTarget :: compileCode() {
  if (int(analyze)) {
    // Generate the command to analyze the VHDL code here.
    StringList command = "";
    if (progNotFound("gvan")) return FALSE;
    command << "gvan -nc " << filePrefix << ".vhdl";
    StringList error = "";
    error << "Could not compile " << filePrefix << ".vhdl";
    if (systemCall(command, error, targetHost)) return FALSE;
  }
  // Return TRUE indicating success.
  return TRUE;
}

// Run the code.
int SimVSSTarget :: runCode() {
  if (int(startup)) {
    // Generate the command to startup the VHDL simulator here.
    StringList command = "";
    StringList sysCommand = "";
    if (int(interactive)) {
      if (progNotFound("vhdldbx")) return FALSE;
      command << "vhdldbx -nc " << filePrefix;
      StringList error = "";
      error << "Could not compile " << filePrefix << ".vhdl";
      if (systemCall(command, error, targetHost)) return FALSE;
    }
    else {
      if (progNotFound("ptvhdlsim")) return FALSE;
      command << "ptvhdlsim -nc -i " << filePrefix << ".com "
	      << filePrefix << "_parts";
      StringList error = "";
      error << "Could not simulate " << filePrefix << ".vhdl";
      if (systemCall(command, error, targetHost)) return FALSE;
    }
    sysCommand << "cd " << (const char*) destDirectory;
    sysCommand << " ; ";

    sysCommand << sysWrapup;
    (void) system(sysCommand);
  }  
  // Return TRUE indicating success.
  return TRUE;
}

ISA_FUNC(SimVSSTarget,VHDLTarget);

// Method called by comm stars to place important code into structure.
void SimVSSTarget :: registerComm(int direction, int pairid, int numxfer,
				  const char* dtype) {
  // direction == 0 --> C2V ; direction == 1 --> V2C.
  // Create a string with the right VHDL data type
  StringList vtype = "";
  StringList name = "";
  if (strcmp(dtype, "INT") == 0) {
    vtype = "INTEGER";
    if (direction) {
      name = "V2Cinteger";
    }
    else {
      name = "C2Vinteger";
    }
  }
  else if (strcmp(dtype, "FLOAT") == 0) {
    vtype = "REAL";
    if (direction) {
      name = "V2Creal";
    }
    else {
      name = "C2Vreal";
    }
  }
  else {
    Error::abortRun(*this, dtype, ": type not supported");
    return;
  }
  
  // Construct unique label and signal names and put comp in main list
  StringList label;
  StringList goName, dataName, doneName;
  StringList rootName = name;
  rootName << pairid;

  label << rootName;
  goName << rootName << "_go";
  dataName << rootName << "_data";
  doneName << rootName << "_done";
  
  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;

  genList->put("pairid", "INTEGER", "", pairid);
  genList->put("numxfer", "INTEGER", "", numxfer);

  portList->put("go", "STD_LOGIC", "IN", goName, NULL);
  if (direction) {
    mainPortList.put(dataName, vtype, "OUT", dataName, NULL);
    topSignalList.put(dataName, vtype, NULL);
    portList->put("data", vtype, "IN", dataName, NULL);
  }
  else {
    portList->put("data", vtype, "OUT", dataName, NULL);
    topSignalList.put(dataName, vtype, NULL);
    mainPortList.put(dataName, vtype, "IN", dataName, NULL);
  }
  portList->put("done", "STD_LOGIC", "OUT", doneName, NULL);

  topCompDeclList.put(label,  portList, genList, name, portList, genList);

  // Also add to port list of main.
  mainPortList.put(goName, "STD_LOGIC", "OUT", goName, NULL);
  topSignalList.put(goName, "STD_LOGIC", NULL);
  topSignalList.put(doneName, "STD_LOGIC", NULL);
  mainPortList.put(doneName, "STD_LOGIC", "IN", doneName, NULL);
}

// Method to write out com file for VSS if needed.
void SimVSSTarget :: setWriteCom() {
  writeCom = 1;
}

// Method to write out com file for VSS if needed.
void SimVSSTarget :: writeComFile() {
  // Make sure to do the com file uniquely too!!!
  StringList comCode = "";
  comCode << "run\n";
  comCode << simWrapup;
  comCode << "quit\n";
  writeFile(comCode, ".com", 0);
}

// Add additional codeStreams.
void SimVSSTarget :: addCodeStreams() {
  addStream("cli_configs", &cli_configs);
  addStream("top_uses", &top_uses);
  addStream("top_entity", &top_entity);
  addStream("top_architecture", &top_architecture);
  addStream("top_configuration", &top_configuration);

  VHDLTarget::addCodeStreams();
}

// Initialize codeStreams.
void SimVSSTarget :: initCodeStreams() {
  cli_configs.initialize();
  top_uses.initialize();
  top_entity.initialize();
  top_architecture.initialize();
  top_configuration.initialize();

  VHDLTarget::initCodeStreams();
}

// Initialize VHDLObjLists.
void SimVSSTarget :: initVHDLObjLists() {
  mainGenList.initialize();
  mainPortList.initialize();
  topSignalList.initialize();
  topCompDeclList.initialize();

  VHDLTarget::initVHDLObjLists();
}
