static const char file_id[] = "SynthTarget.cc";
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

 Target for VHDL code generation and handoff to Synopsys synthesis.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SynthTarget.h"
#include "KnownTarget.h"
#include "paths.h"

// Constructor.
SynthTarget :: SynthTarget(const char* name,const char* starclass,
			 const char* desc) :
StructTarget(name,starclass,desc) {
  addState(elaborate.setState("elaborate",this,"YES",
			    "switch for elaborating design into structure."));
  addState(compile.setState("compile",this,"YES",
			    "switch for compiling structure into gates."));
}

// Clone the Target.
Block* SynthTarget :: makeNew() const {
  LOG_NEW; return new SynthTarget(name(), starType(), descriptor());
}

static SynthTarget proto("Synth-VHDL", "VHDLStar",
			 "VHDL code generation target for Synopsys");
static KnownTarget entry(proto,"Synth-VHDL");

// Write the code to a file.
void SynthTarget :: writeCode() {
  writeFile(myCode,".vhdl",displayFlag);

  // FIXME: Need to do this inside before code is generated instead.
  // Change all integers to 4-bit types to simplify synthesis.
  StringList command = "";
  command << "cd " << (const char*) destDirectory;
  command << " ; ";
  command << "rm -f " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "sed s/\" INTEGER\"/\" INTEGER range 0 to 15\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  command << "sed s/\" integer\"/\" INTEGER range 0 to 15\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  system(command);
}

// Write the command script to a file.
int SynthTarget :: compileCode() {
  // Generate the Synopsys design_analyer command script file here.

  if (int(analyze)) {
  StringList comCode = "";
  comCode << headerComment("/* \n * ","\n */\n"," * ");

  comCode << "/* Synopsys design_analyzer Command Script */\n";

  comCode << "\n";
  comCode << "/* Set default variables. */\n";
  comCode << "\n";
  comCode << "designer = \"" << cuserid(NULL) << "\"" << "\n";
  comCode << "temp = get_unix_variable(\"SYNOPSYS\")" << "\n";
  comCode << "search_path = \". \" + temp + \"/libraries/syn\"" << "\n";
  comCode << "link_library = \"* class.db\"" << "\n";
  comCode << "target_library = \"class.db\"" << "\n";
  comCode << "symbol_library = \"class.sdb\"" << "\n";
  comCode << "default_schematic_options = \"-size infinite\"" << "\n";
  // This is obsolete with v3.3a.
  //  comCode << "hdlin_source_to_gates_mode = \"high\"" << "\n";

  comCode << "\n";
  comCode << "/* Move to the working directory. */\n";
  comCode << "\n";
  comCode << "sh cd " << (const char*) destDirectory << "\n";

  comCode << "\n";
  comCode << "/* Analyze the VHDL design file. */\n";
  comCode << "\n";
  comCode << "analyze -format vhdl -lib WORK {\""
	  << filePrefix << ".vhdl" << "\"}" << "\n"; 

  if (int(elaborate)) {
  comCode << "\n";
  comCode << "/* Elaborate the design. */\n";
  comCode << "\n";
  comCode << "elaborate " << filePrefix
	  << " -arch \"structure\" -lib WORK -update" << "\n";

  comCode << "\n";
  comCode << "/* Create schematics for viewing the design. */\n";
  comCode << "\n";
  comCode << "create_schematic -size infinite -gen_database" << "\n";
  comCode << "create_schematic -size infinite -symbol_view" << "\n";
  comCode << "create_schematic -size infinite -hier_view" << "\n";
  comCode << "create_schematic -size infinite -schematic_view" << "\n";

  if (int(compile)) {
  comCode << "\n";
  comCode << "/* Uniquify the design hierarchy to prepare for mapping. */\n";
  comCode << "\n";
  comCode << "uniquify" << "\n";

  comCode << "\n";
  comCode << "/* Compile the design to a gate-mapped version. */\n";
  comCode << "\n";
  comCode << "compile -map_effort medium" << "\n";

  comCode << "\n";
  comCode << "/* Set the current design unit. */\n";
  comCode << "\n";
  comCode << "current_design = \"" << filePrefix << ".db:" << filePrefix
	  << "\"" << "\n";

  comCode << "\n";
  comCode << "/* Create schematics for viewing the design. */\n";
  comCode << "\n";
  comCode << "create_schematic -size infinite -gen_database" << "\n";
  comCode << "create_schematic -size infinite -schematic_view"
	  << " -symbol_view -hier_view" << "\n";

  if (int(report)) {
  comCode << "\n";
  comCode << "/* Generate reports on area and timing. */\n";
  comCode << "\n";
  comCode << "report_area" << "\n";
  comCode << "report_timing -path full -delay max -max_paths 1 -nworst 1" << "\n";
}
}
}  
  writeFile(comCode, ".com", displayFlag);
}
  // Return TRUE indicating success.
  return TRUE;
}

// Run the code.
int SynthTarget :: runCode() {
  if (int(analyze)) {
    // Startup Synopsys design_analyzer with the command script file here.
    StringList command = "";
    if (progNotFound("design_analyzer")) return FALSE;
    command << "design_analyzer -f " << filePrefix << ".com" << " &";
    StringList error = "";
    error << "Could not analyze " << filePrefix << ".vhdl";
    if (systemCall(command, error, targetHost)) return FALSE;
  }
  // Return TRUE indicating success.
  return TRUE;
}

ISA_FUNC(SynthTarget,StructTarget);
