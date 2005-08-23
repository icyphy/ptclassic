static const char file_id[] = "SynthArchTarget.cc";
/******************************************************************
Version identification:
@(#)SynthArchTarget.cc	1.8 10/04/99

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

 Target for VHDL code generation and handoff to Synopsys synthesis.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SynthArchTarget.h"
#include "KnownTarget.h"
#include "paths.h"
#include <stdio.h>              // cuserid()
#include <unistd.h>             // cuserid() under NT

// Constructor.
SynthArchTarget :: SynthArchTarget(const char* name,const char* starclass,
			 const char* desc) :
ArchTarget(name,starclass,desc) {
  addState(elaborate.setState("elaborate",this,"YES",
			    "Switch for elaborating design into structure."));
  addState(compile.setState("compile",this,"YES",
			    "Switch for compiling structure into gates."));
  addState(precision.setState("precision",this,"INTEGER RANGE 0 to 15",
			      "Specifies precision for synthesis."));
}

// Clone the Target.
Block* SynthArchTarget :: makeNew() const {
  LOG_NEW; return new SynthArchTarget(name(), starType(), descriptor());
}

static SynthArchTarget proto("SynthArch-VHDL", "VHDLStar",
			 "VHDL code generation target for Synopsys");
static KnownTarget entry(proto,"SynthArch-VHDL");


void SynthArchTarget :: setup() {
    precision.initialize();
    StringList precisionString;
    precisionString = precision.currentValue();
    precSpec = savestring(precisionString);

    ArchTarget::setup();
}

// Write the code to a file.
void SynthArchTarget :: writeCode() {
    //  writeFile(myCode,".vhdl",displayFlag);
    ArchTarget::writeCode();

  // HACK: Change all REALs to INTEGERs.
  StringList command = "";
  command << "cd " << (const char*) destDirectory;
  command << " ; ";
  command << "rm -f " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  // REAL; -> INTEGER;
  command << "sed s/\" REAL;\"/\" INTEGER;\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  // real; -> INTEGER;
  command << "sed s/\" real;\"/\" INTEGER;\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  // IN REAL -> IN INTEGER
  command << "sed s/\"IN REAL\"/\"IN INTEGER\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  // OUT REAL -> OUT INTEGER
  command << "sed s/\"OUT REAL\"/\"OUT INTEGER\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  // in REAL -> IN INTEGER
  command << "sed s/\"in REAL\"/\"IN INTEGER\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  // out REAL -> OUT INTEGER
  command << "sed s/\"out REAL\"/\"OUT INTEGER\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  // : REAL -> : INTEGER
  command << "sed s/\": REAL\"/\": INTEGER\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  // .0 -> 0
  command << "sed s/\"[.]0\"/\"0\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  // 0. -> 0
  command << "sed s/\"0[.]\"/\"0\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  // -0 -> 0
  command << "sed s/\"-0\"/\"0\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  // -1 -> 1
  command << "sed s/\"-1\"/\"1\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  // float -> INTEGER
  command << "sed s/\" float;\"/\" INTEGER;\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  system(command);

  // FIXME: Need to do this inside before code is generated instead.
  // Change all integers to 4-bit types to simplify synthesis.
  //  StringList command = "";
  command = "";
  command << "cd " << (const char*) destDirectory;
  command << " ; ";
  command << "rm -f " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  //  command << "sed s/\" INTEGER\"/\" INTEGER range -16 to 15\"/g ";
  command << "sed s/\" INTEGER\"/\" " << precSpec << "\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  command << " ; ";
  //  command << "sed s/\" integer\"/\" INTEGER range -16 to 15\"/g ";
  command << "sed s/\" integer\"/\" " << precSpec << "\"/g ";
  command << filePrefix << ".vhdl" << " > " << "temp" << filePrefix << ".vhdl";
  command << " ; ";
  command << "mv -f " << "temp" << filePrefix << ".vhdl" << " "
	  << filePrefix << ".vhdl";
  system(command);
}

// Write the command script to a file.
int SynthArchTarget :: compileCode() {
  // Generate the Synopsys synthesis command script file here.

  if (int(analyze)) {
    StringList comCode = "";
    comCode << headerComment("/* \n * ","\n */\n"," * ");

    comCode << "/* Synopsys Synthesis Command Script */\n";

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
	  comCode << "report_area > " << filePrefix << ".area.rep" << "\n";
	  comCode << "report_timing -path full -delay max -max_paths 1"
		  << " -nworst 1" << "\n";

	  comCode << report_code;
	}
      }
    }  
    writeFile(comCode, ".com", displayFlag);
  }
  // Return TRUE indicating success.
  return TRUE;
}

// Run the code.
int SynthArchTarget :: runCode() {
  if (analyze) {
    // If interactive, start Synopsys design_analyzer (with GUI).
    // If not, start Synopsys dc_shell (shell).
    if (interactive) {
      // Startup Synopsys design_analyzer with the command script file here.
      StringList command = "";
      if (progNotFound("design_analyzer")) return FALSE;
      //    command << "design_analyzer -f " << filePrefix << ".com" << " &";
      command << "design_analyzer -f " << filePrefix << ".com";
      StringList error = "";
      error << "Could not analyze " << filePrefix << ".vhdl";
      if (systemCall(command, error, targetHost)) return FALSE;
    }
    else {
      // Startup Synopsys dc_shell with the command script file here.
      StringList command = "";
      if (progNotFound("dc_shell")) return FALSE;
      //    command << "dc_shell -f " << filePrefix << ".com" << " &";
      command << "dc_shell -f " << filePrefix << ".com";
      StringList error = "";
      error << "Could not analyze " << filePrefix << ".vhdl";
      if (systemCall(command, error, targetHost)) return FALSE;
    }

    // Look at the report file, if there is one, and announce the area.
    if (int(report)) {
	/*
      StringList fileName = "";
      fileName << filePrefix << ".rep";
      StringList command = "";
      command << "cd " << (const char*) destDirectory;
      command << " ; ";
      // Note:  using sh shell.
      // Must use case-sensitive input to grep.
      // Use > output redirector, which in sh can overwrite existing file.
      command << "grep Total " << fileName << " > " << "DOH";
      StringList error = "";
      error = "Problem looking at report file";
      if (systemCall(command, error, targetHost)) return FALSE;
      */
    }
  }
  // Return TRUE indicating success.
  return TRUE;
}

ISA_FUNC(SynthArchTarget,ArchTarget);
