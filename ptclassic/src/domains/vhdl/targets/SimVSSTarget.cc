static const char file_id[] = "SimVSSTarget.cc";
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

 Programmer: Michael C. Williamson

 Target for VHDL code generation and interaction with Synopsys simulator.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SimVSSTarget.h"
#include "KnownTarget.h"
#include "paths.h"

// Constructor.
SimVSSTarget :: SimVSSTarget(const char* name,const char* starclass,
			     const char* desc) :
VHDLTarget(name,starclass,desc) {
  addState(synopsys.setState("$SYNOPSYS",this,"/usr/tools/synopsys",
			    "value for SYNOPSYS environment variable."));
  // Needed by the "gvan" vhdl code analyzer.
  addState(arch.setState("$ARCH",this,"sparcOS5",
			    "value for ARCH environment variable."));
  // Needed by the "(pt)vhdlsim" vhdl simulator.
  addState(simarch.setState("$SIM_ARCH",this,"sparcOS5",
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
//  printf("Begin Method of SimVSSTarget called\n");

  // Init members here to avoid erasing code and items created
  // during begin method of stars.
  VHDLTarget::setup();

  initVHDLObjLists();
  initCodeStreams();

  VHDLTarget::begin();
}

void SimVSSTarget :: setup() {
//  printf("Setup Method of SimVSSTarget called\n");

  synopsys.initialize();
  arch.initialize();
  simarch.initialize();

  StringList synopsysString;
  StringList archString;
  StringList simarchString;

  synopsysString = "SYNOPSYS=";
  synopsysString << synopsys.currentValue();
  char* saveSynopsysString = savestring(synopsysString);

  archString = "ARCH=";
  archString << arch.currentValue();
  char* saveArchString = savestring(archString);

  simarchString = "SIM_ARCH=";
  simarchString << simarch.currentValue();
  char* saveSimarchString = savestring(simarchString);

  // Under SunOS4.1.3, the man page for putenv() says:
  //    string points to a string of the form `name=value'  putenv()
  //    makes  the  value  of the environment variable name equal to
  //    value by altering an existing variable  or  creating  a  new
  //    one.   In  either  case,  the  string  pointed  to by string
  //    becomes part of the environment, so altering the string will
  //    change  the  environment.   The  space  used by string is no
  //    longer used once a new string-defining  name  is  passed  to
  //    putenv().

  putenv(saveSynopsysString);
  putenv(saveArchString);
  putenv(saveSimarchString);

  writeCom = 1;

// Don't init code streams here because it clobbers VHDLCSend,Receive
// stars' code generated in their begin methods.
//  VHDLTarget::setup();
//  initVHDLObjLists();
//  initCodeStreams();
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

  top_uses << "-- top-level use clauses\n";
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

  top_architecture << "-- top-level architecture\n";
  top_architecture << "architecture ";
  top_architecture << "structure";
  top_architecture << " of ";
  top_architecture << topName;
  top_architecture << " is\n";
  top_architecture << cli_comps;
  top_architecture << "component ";
  top_architecture << galName;
  top_architecture << "\n";
  top_architecture << addGenericRefs(&mainGenList);
  top_architecture << addPortRefs(&mainPortList);
  top_architecture << "\n";
  top_architecture << "end component;\n";
  top_architecture << "\n";
  top_architecture << addSignalDeclarations(&topSignalList);
  top_architecture << "\n";
  top_architecture << "begin\n";

  StringList label = galName;
  label << "_proc";
  registerCompMap(label, galName, &mainPortMapList, &mainGenMapList);

  top_architecture << addComponentMaps(&topCompMapList);
  top_architecture << "end ";
  top_architecture << "structure";
  top_architecture << ";\n";

  // Generate the top-level configuration.
  top_configuration << "-- top-level configuration\n";
  top_configuration << "configuration ";
  top_configuration << "parts";
  top_configuration << " of ";
  top_configuration << topName;
  top_configuration << " is\n";
  top_configuration << "for structure\n";
  top_configuration << indent(1) << "for all:" << galName;
  top_configuration << " use entity work." << galName;
  top_configuration << "(behavior); end for;\n";
  top_configuration << cli_configs;
  top_configuration << "end for;\n";
  top_configuration << "end parts;\n";

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
  
  code << "\n" << cli_models;
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
      command << "ptvhdlsim -nc -i " << filePrefix << ".com " << filePrefix;
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

// Add in signal declarations here from signalList.
StringList SimVSSTarget :: addSignalDeclarations(VHDLSignalList* signalList) {
  StringList all;
  int level = 0;
  VHDLSignalListIter nextSignal(*signalList);
  VHDLSignal* signal;
  while ((signal = nextSignal++) != 0) {
    all << indent(level) << "signal " << signal->name << ": "
	<< signal->type << ";\n";
  }
  return all;
}

// Add in component mappings here from compMapList.
StringList SimVSSTarget :: addComponentMaps(VHDLCompMapList* compMapList) {
  int level = 0;
  StringList all;
  VHDLCompMapListIter nextCompMap(*compMapList);
  VHDLCompMap* compMap;
  while ((compMap = nextCompMap++) != 0) {
    level++;
    all << indent(level) << compMap->name << ": "
		       << compMap->type << "\n";

    // Add in generic maps here from genMapList.
    if (compMap->genMapList->head()) {
      level++;
      all << indent(level) << "generic map(\n";
      VHDLGenericMapListIter nextGenMap(*(compMap->genMapList));
      VHDLGenericMap* ngenmap;
      int genCount = 0;
      while ((ngenmap = nextGenMap++) != 0) {
	level++;
	if (genCount) {
	  all << ",\n";
	}
	all << indent(level) << ngenmap->name << " => "
			   << ngenmap->mapping;
	genCount++;
	level--;
      }
      all << "\n";
      all << indent(level) << ")\n";
      level--;
    }

    // Add in port maps here from portMapList.
    if (compMap->portMapList->head()) {
      level++;
      all << indent(level) << "port map(\n";
      VHDLPortMapListIter nextPortMap(*(compMap->portMapList));
      VHDLPortMap* nportmap;
      int portCount = 0;
      while ((nportmap = nextPortMap++) != 0) {
	level++;
	if (portCount) {
	  all << ",\n";
	}
	all << indent(level) << nportmap->name << " => "
			   << nportmap->mapping;
	portCount++;
	level--;
      }
      all << "\n";
      all << indent(level) << ")\n";
      level--;
    }
    
    all << indent(level) << ";\n";
    level--;
  }
  return all;
}

// Register component mapping.
void SimVSSTarget :: registerCompMap(StringList name, StringList type,
				     VHDLPortMapList* portMapList,
				     VHDLGenericMapList* genMapList) {
  // Allocate memory for a new VHDLCompMap and put it in the list.
  VHDLCompMap* newCompMap = new VHDLCompMap;
  newCompMap->setName(name);
  newCompMap->type = type;
  newCompMap->genMapList = genMapList;
  newCompMap->portMapList = portMapList;
  topCompMapList.put(*newCompMap);
}

// Method called by C2V star to place important code into structure.
void SimVSSTarget :: registerC2V(int pairid, int numxfer, const char* dtype) {
//  printf("RegisterC2V Method of SimVSSTarget called\n");

  // Create a string with the right VHDL data type
  StringList vtype = "";
  StringList name = "";
  if (strcmp(dtype, "INT") == 0) {
    vtype = "INTEGER";
    name = "C2Vinteger";
  }
  else if (strcmp(dtype, "FLOAT") == 0) {
    vtype = "REAL";
    name = "C2Vreal";
  }
  else
    Error::abortRun(*this, dtype, ": type not supported");
  
  // Construct unique label and signal names and put comp map in main list
  StringList label;
  StringList goName, dataName, doneName;
  StringList rootName = name;
  rootName << pairid;

  label << rootName;
  goName << rootName << "_go";
  dataName << rootName << "_data";
  doneName << rootName << "_done";
  
  VHDLGenericMapList* genMapList = new VHDLGenericMapList;
  VHDLPortMapList* portMapList = new VHDLPortMapList;
  genMapList->initialize();
  portMapList->initialize();
  
  genMapList->put("pairid", pairid);
  genMapList->put("numxfer", numxfer);
  portMapList->put("go", goName);
  portMapList->put("data", dataName);
  portMapList->put("done", doneName);
  registerCompMap(label, name, portMapList, genMapList);

  // Also add to port list of main.
  mainPortList.put(goName, "OUT", "STD_LOGIC");
  mainPortList.put(dataName, "IN", vtype);
  mainPortList.put(doneName, "IN", "STD_LOGIC");
  // Also add to port map list of main.
  mainPortMapList.put(goName, goName);
  mainPortMapList.put(dataName, dataName);
  mainPortMapList.put(doneName, doneName);
  // Also add to signal list of top.
  topSignalList.put(goName, "STD_LOGIC", goName, goName);
  topSignalList.put(dataName, vtype, dataName, dataName);
  topSignalList.put(doneName, "STD_LOGIC", doneName, doneName);
}

// Method called by V2C star to place important code into structure.
void SimVSSTarget :: registerV2C(int pairid, int numxfer, const char* dtype) {
//  printf("RegisterV2C Method of SimVSSTarget called\n");

  // Create a string with the right VHDL data type
  StringList vtype = "";
  StringList name = "";
  if (strcmp(dtype, "INT") == 0) {
    vtype = "INTEGER";
    name = "V2Cinteger";
  }
  else if (strcmp(dtype, "FLOAT") == 0) {
    vtype = "REAL";
    name = "V2Creal";
  }
  else
    Error::abortRun(*this, dtype, ": type not supported");
  
  // Construct unique label and signal names and put comp map in main list
  StringList label;
  StringList goName, dataName, doneName;
  StringList rootName = name;
  rootName << pairid;

  label << rootName;
  goName << rootName << "_go";
  dataName << rootName << "_data";
  doneName << rootName << "_done";
  
  VHDLGenericMapList* genMapList = new VHDLGenericMapList;
  VHDLPortMapList* portMapList = new VHDLPortMapList;
  genMapList->initialize();
  portMapList->initialize();
  
  genMapList->put("pairid", pairid);
  genMapList->put("numxfer", numxfer);
  portMapList->put("go", goName);
  portMapList->put("data", dataName);
  portMapList->put("done", doneName);
  registerCompMap(label, name, portMapList, genMapList);

  // Also add to port list of main.
  mainPortList.put(goName, "OUT", "STD_LOGIC");
  mainPortList.put(dataName, "OUT", vtype);
  mainPortList.put(doneName, "IN", "STD_LOGIC");
  // Also add to port map list of main.
  mainPortMapList.put(goName, goName);
  mainPortMapList.put(dataName, dataName);
  mainPortMapList.put(doneName, doneName);
  // Also add to signal list of top.
  topSignalList.put(goName, "STD_LOGIC", goName, goName);
  topSignalList.put(dataName, vtype, dataName, dataName);
  topSignalList.put(doneName, "STD_LOGIC", doneName, doneName);
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
  comCode << "quit\n";
  writeFile(comCode, ".com", 0);
}

// Add additional codeStreams.
void SimVSSTarget :: addCodeStreams() {
  addStream("cli_models", &cli_models);
  addStream("cli_comps", &cli_comps);
  addStream("cli_configs", &cli_configs);
  addStream("top_uses", &top_uses);
  addStream("top_entity", &top_entity);
  addStream("top_architecture", &top_architecture);
  addStream("top_configuration", &top_configuration);
}

// Initialize codeStreams.
void SimVSSTarget :: initCodeStreams() {
  cli_models.initialize();
  cli_comps.initialize();
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
  mainGenMapList.initialize();
  mainPortMapList.initialize();
  topSignalList.initialize();
  topCompMapList.initialize();

  VHDLTarget::initVHDLObjLists();
}
