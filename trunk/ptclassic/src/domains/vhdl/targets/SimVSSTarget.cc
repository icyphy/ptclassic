static const char file_id[] = "SimVSSTarget.cc";
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

 Target for VHDL code generation and interaction with Synopsys simulator.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SimVSSTarget.h"
#include "KnownTarget.h"
#include <ostream.h>

// Constructor.
SimVSSTarget :: SimVSSTarget(const char* name,const char* starclass,
			     const char* desc) :
VHDLTarget(name,starclass,desc) {
  addState(simarch.setState("simarch",this,"sparcOS5",
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

//  addStream("preSynch", &preSynch);
//  addStream("postSynch", &postSynch);
  needC2V = 0;
  needV2C = 0;
  pairNumber = 0;
}

// Clone the Target.
Block* SimVSSTarget :: makeNew() const {
  LOG_NEW; return new SimVSSTarget(name(), starType(), descriptor());
}

static SimVSSTarget proto("SimVSS-VHDL", "VHDLStar",
  "VHDL code generation target for Synopsys simulation");
static KnownTarget entry(proto,"SimVSS-VHDL");

void SimVSSTarget :: setup() {
  needC2V = 0;
  needV2C = 0;
  pairNumber = 0;

  // Generate the command to set the SIM_ARCH environment variable here.
  StringList command = "";
  command << "set SIM_ARCH = " << (const char*) simarch;
  command << " ; ";
  command << "echo SIM_ARCH = $SIM_ARCH";
  system(command);

  VHDLTarget :: setup();
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
  StringList prNum = pairNumber;
  pair.cgcStar->setState("pairNumber", hashstring(prNum));
  pair.cgStar->setState("pairNumber", hashstring(prNum));
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
  StringList galName = filePrefix;
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
  top_architecture << "component C2V\n";
  top_architecture << indent(1) << "generic ( pairid  : INTEGER ;\n";
  top_architecture << indent(1) << "          numxfer : INTEGER );\n";
  top_architecture << indent(1) << "port ( go   : in  STD_LOGIC ;\n";
  top_architecture << indent(1) << "       data : out INTEGER   ;\n";
  top_architecture << indent(1) << "       done : out STD_LOGIC );\n";
  top_architecture << "end component;\n";
  top_architecture << "\n";
  top_architecture << "component V2C\n";
  top_architecture << indent(1) << "generic ( pairid  : INTEGER ;\n";
  top_architecture << indent(1) << "          numxfer : INTEGER );\n";
  top_architecture << indent(1) << "port ( go   : in  STD_LOGIC ;\n";
  top_architecture << indent(1) << "       data : in  INTEGER   ;\n";
  top_architecture << indent(1) << "       done : out STD_LOGIC );\n";
  top_architecture << "end component;\n";
  top_architecture << "\n";
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
  if (needC2V) {
  top_configuration << indent(1) << "for all:C2V use entity work.C2V";
  top_configuration << "(CLI); end for;\n";
}
  if (needV2C) {
  top_configuration << indent(1) << "for all:V2C use entity work.V2C";
  top_configuration << "(CLI); end for;\n";
}
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
  
  code << "\n
--C2V.vhdl

library SYNOPSYS,IEEE;
use SYNOPSYS.ATTRIBUTES.all;
use IEEE.STD_LOGIC_1164.all;

entity C2V is
	generic ( pairid	: INTEGER	;
		  numxfer	: INTEGER	);
	port	( go		: in STD_LOGIC	;
		  data		: out INTEGER	;
		  done		: out STD_LOGIC	);
end C2V;

architecture CLI of C2V is

	attribute FOREIGN of CLI : architecture is \"Synopsys:CLI\";

	attribute CLI_ELABORATE	of CLI	: architecture is \"c2v_open\";
	attribute CLI_EVALUATE	of CLI	: architecture is \"c2v_eval\";
	attribute CLI_ERROR	of CLI	: architecture is \"c2v_error\";
	attribute CLI_CLOSE	of CLI	: architecture is \"c2v_close\";

	attribute CLI_PIN	of go	: signal is CLI_ACTIVE;
	attribute CLI_PIN	of data	: signal is CLI_PASSIVE;
	attribute CLI_PIN	of done	: signal is CLI_PASSIVE;

begin
end;
";

  code << "\n
--V2C.vhdl

library SYNOPSYS,IEEE;
use SYNOPSYS.ATTRIBUTES.all;
use IEEE.STD_LOGIC_1164.all;

entity V2C is
	generic ( pairid	: INTEGER	;
		  numxfer	: INTEGER	);
	port	( go		: in STD_LOGIC	;
		  data		: in INTEGER	;
		  done		: out STD_LOGIC	);
end V2C;

architecture CLI of V2C is

	attribute FOREIGN of CLI : architecture is \"Synopsys:CLI\";

	attribute CLI_ELABORATE	of CLI	: architecture is \"v2c_open\";
	attribute CLI_EVALUATE	of CLI	: architecture is \"v2c_eval\";
	attribute CLI_ERROR	of CLI	: architecture is \"v2c_error\";
	attribute CLI_CLOSE	of CLI	: architecture is \"v2c_close\";

	attribute CLI_PIN	of go	: signal is CLI_ACTIVE;
	attribute CLI_PIN	of done	: signal is CLI_PASSIVE;
	attribute CLI_PIN	of data	: signal is CLI_PASSIVE;

begin
end;
";

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
//  code << preSynch;
  code << myCode;
//  code << postSynch;
  
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
}

// Write the command log to a file.
int SimVSSTarget :: compileCode() {
  if (int(analyze)) {
    // Generate the command to analyze the VHDL code here.
    StringList command = "";
    command << "cd " << (const char*) destDirectory;
    command << " ; ";
    command << "gvan " << filePrefix << ".vhdl";
    system(command);
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
  // Return TRUE indicating success.
  return TRUE;
}

ISA_FUNC(SimVSSTarget,VHDLTarget);


// Add in generic refs here from genList.
StringList SimVSSTarget :: addGenericRefs(VHDLGenericList* genList) {
  StringList all, opener, body, closer;
  int level = 0;
  level++;
  opener << indent(level) << "generic(\n";

  int genCount = 0;
  VHDLGenericListIter nextGeneric(*genList);
  VHDLGeneric* ngen;
  while ((ngen = nextGeneric++) != 0) {
    level++;
    if (genCount) {
      body << ";\n";
    }
    body << indent(level) << ngen->name << ": " << ngen->type;
    if (strlen(ngen->defaultVal) > 0) {
      body << " := " << ngen->defaultVal;
    }
    genCount++;
    level--;
  }
  closer << "\n";
  closer << indent(level) << ");\n";
  level--;

  if (genCount) {
    all << opener << body << closer;
  }
  return(all);
}

// Add in port refs here from portList.
StringList SimVSSTarget :: addPortRefs(VHDLPortList* portList) {
  StringList all, opener, body, closer;
  int level = 0;
  level++;
  opener << indent(level) << "port(\n";

  int portCount = 0;
  VHDLPortListIter nextPort(*portList);
  VHDLPort* nport;
  while ((nport = nextPort++) != 0) {
    level++;
    if (portCount) {
      body << ";\n";
    }
    body << indent(level) << nport->name << ": " << nport->direction
	 << " " << nport->type;
    portCount++;
    level--;
  }
  closer << "\n";
  closer << indent(level) << ");\n";
  level--;

  if (portCount) {
    all << opener << body << closer;
  }
  return(all);
}

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
  return(all);
}

// Add in component mappings here from compMapList.
StringList SimVSSTarget :: addComponentMaps(VHDLCompMapList* compMapList) {
  int level = 0;
  StringList all;
  VHDLCompMapListIter nextCompMap(*compMapList);
  VHDLCompMap* compMap;
  while ((compMap = nextCompMap++) != 0) {
    level++;
    all << indent(level) << compMap->label << ": "
		       << compMap->name << "\n";

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
  return(all);
}

// Register component mapping.
void SimVSSTarget :: registerCompMap(StringList label, StringList name,
				     VHDLPortMapList* portMapList,
				     VHDLGenericMapList* genMapList) {
//  Problem: inList uses name, not label, for generic VHDL objects.
//  Will just comment this out for now.  If get repeated registrations
//  of a comp map, will deal with that.
//  if (compMapList.inList(label)) return;
  
  // Allocate memory for a new VHDLCompMap and put it in the list.
  VHDLCompMap* newCompMap = new VHDLCompMap;
  newCompMap->label = label;
  newCompMap->name = name;
  newCompMap->genMapList = genMapList;
  newCompMap->portMapList = portMapList;
  topCompMapList.put(*newCompMap);
}

// Method called by C2V star to place important code into structure.
void SimVSSTarget :: registerC2V(int pairid, int numxfer) {
  needC2V = 1;
  // Construct unique label and signal names and put comp map in main list
  StringList label;
  StringList name;
  StringList goName, dataName, doneName;
  name = "C2V";
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
  mainPortList.put(dataName, "IN", "INTEGER");
  mainPortList.put(doneName, "IN", "STD_LOGIC");
  // Also add to port map list of main.
  mainPortMapList.put(goName, goName);
  mainPortMapList.put(dataName, dataName);
  mainPortMapList.put(doneName, doneName);
  // Also add to signal list of top.
  topSignalList.put(goName, "STD_LOGIC", goName, goName);
  topSignalList.put(dataName, "INTEGER", dataName, dataName);
  topSignalList.put(doneName, "STD_LOGIC", doneName, doneName);
}

// Method called by V2C star to place important code into structure.
void SimVSSTarget :: registerV2C(int pairid, int numxfer) {
  needV2C = 1;
  // Construct unique label and signal names and put comp map in main list
  StringList label;
  StringList name;
  StringList goName, dataName, doneName;
  name = "V2C";
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
  mainPortList.put(dataName, "OUT", "INTEGER");
  mainPortList.put(doneName, "IN", "STD_LOGIC");
  // Also add to port map list of main.
  mainPortMapList.put(goName, goName);
  mainPortMapList.put(dataName, dataName);
  mainPortMapList.put(doneName, doneName);
  // Also add to signal list of top.
  topSignalList.put(goName, "STD_LOGIC", goName, goName);
  topSignalList.put(dataName, "INTEGER", dataName, dataName);
  topSignalList.put(doneName, "STD_LOGIC", doneName, doneName);
}
