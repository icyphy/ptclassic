static const char file_id[] = "SynthTarget.cc";
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

 Target for VHDL code generation and handoff to Synopsys synthesis.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SynthTarget.h"
#include "KnownTarget.h"
#include <ostream.h>

// Constructor.
SynthTarget :: SynthTarget(const char* name,const char* starclass,
			 const char* desc) :
StructTarget(name,starclass,desc) {
  addState(analyze.setState("analyze",this,"TRUE",
			    "switch for analyzing code into Synopsys."));
  addState(elaborate.setState("elaborate",this,"TRUE",
			    "switch for elaborating design into structure."));
  addState(compile.setState("compile",this,"TRUE",
			    "switch for compiling structure into gates."));
  addState(report.setState("report",this,"TRUE",
			    "switch for generating area & timing reports."));
}

// Clone the Target.
Block* SynthTarget :: makeNew() const {
  LOG_NEW; return new SynthTarget(name(), starType(), descriptor());
}

static SynthTarget proto("Synth-VHDL", "CGStar",
			 "VHDL code generation target for Synopsys");
static KnownTarget entry(proto,"Synth-VHDL");

// Write the code to a file.
void SynthTarget :: writeCode() {
  writeFile(myCode,".vhdl",displayFlag);

  // Change all integers to 4-bit types to simplify synthesis.
  StringList command = "";
  command << "cd " << (const char*) destDirectory;
  command << " ; ";
  command << "rm -f " << "temp" << filePrefix << ".vhdl";
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
}

// Write the command log to a file.
int SynthTarget :: compileCode() {
  // Generate the Synopsys design_analyer command log file here.

  if (int(analyze)) {
  StringList logCode = "";
  logCode << headerComment("/* \n * ","\n */\n"," * ");

  logCode << "/* Synopsys design_analyzer Command Script */\n";

  logCode << "\n";
  logCode << "/* Set default variables. */\n";
  logCode << "\n";
  logCode << "designer = \"" << cuserid(NULL) << "\"" << "\n";
  logCode << "temp = get_unix_variable(\"SYNOPSYS\")" << "\n";
  logCode << "search_path = \". \" + temp + \"/libraries/syn\"" << "\n";
  logCode << "link_library = \"* class.db\"" << "\n";
  logCode << "target_library = \"class.db\"" << "\n";
  logCode << "symbol_library = \"class.sdb\"" << "\n";
  logCode << "default_schematic_options = \"-size infinite\"" << "\n";
  logCode << "hdlin_source_to_gates_mode = \"high\"" << "\n";

  logCode << "\n";
  logCode << "/* Move to the working directory. */\n";
  logCode << "\n";
  logCode << "sh cd " << (const char*) destDirectory << "\n";

  logCode << "\n";
  logCode << "/* Analyze the VHDL design file. */\n";
  logCode << "\n";
  logCode << "analyze -format vhdl -lib WORK {\""
	  << filePrefix << ".vhdl" << "\"}" << "\n"; 

  if (int(elaborate)) {
  logCode << "\n";
  logCode << "/* Elaborate the design. */\n";
  logCode << "\n";
  logCode << "elaborate " << filePrefix
	  << " -arch \"structure\" -lib WORK -update" << "\n";

  logCode << "\n";
  logCode << "/* Create schematics for viewing the design. */\n";
  logCode << "\n";
  logCode << "create_schematic -size infinite -gen_database" << "\n";
  logCode << "create_schematic -size infinite -symbol_view" << "\n";
  logCode << "create_schematic -size infinite -hier_view" << "\n";
  logCode << "create_schematic -size infinite -schematic_view" << "\n";

  if (int(compile)) {
  logCode << "\n";
  logCode << "/* Uniquify the design hierarchy to prepare for mapping. */\n";
  logCode << "\n";
  logCode << "uniquify" << "\n";

  logCode << "\n";
  logCode << "/* Compile the design to a gate-mapped version. */\n";
  logCode << "\n";
  logCode << "compile -map_effort medium" << "\n";

  logCode << "\n";
  logCode << "/* Set the current design unit. */\n";
  logCode << "\n";
  logCode << "current_design = \"" << filePrefix << ".db:" << filePrefix
	  << "\"" << "\n";

  logCode << "\n";
  logCode << "/* Create schematics for viewing the design. */\n";
  logCode << "\n";
  logCode << "create_schematic -size infinite -gen_database" << "\n";
  logCode << "create_schematic -size infinite -schematic_view"
	  << " -symbol_view -hier_view" << "\n";

  if (int(report)) {
  logCode << "\n";
  logCode << "/* Generate reports on area and timing. */\n";
  logCode << "\n";
  logCode << "report_area" << "\n";
  logCode << "report_timing -path full -delay max -max_paths 1 -nworst 1" << "\n";
}
}
}  
  writeFile(logCode,".log",displayFlag);
}
  // Return TRUE indicating success.
  return TRUE;
}

// Run the code.
int SynthTarget :: runCode() {
  if (int(analyze)) {
  // Startup Synopsys design_analyzer with the command log file here.
  StringList daCommand = "";
  daCommand << "cd " << (const char*) destDirectory << ";";
  daCommand << "design_analyzer -f " << filePrefix << ".log" << " &";
  system(daCommand);
}  
  // Return TRUE indicating success.
  return TRUE;
}

ISA_FUNC(SynthTarget,StructTarget);
