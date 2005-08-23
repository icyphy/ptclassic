static const char file_id[] = "CompileTarget.cc";
/******************************************************************
Version identification:
@(#)CompileTarget.cc	1.71	07/22/98

Copyright (c) 1990-1996 The Regents of the University of California.
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

 Programmer:  E. A. Lee
 Date of creation: 12/8/91

WTC/BLE: Tcl/Tk initialization and invocation of begin methods 8/1/95
BLE: Updated code generation to reflect changes in sanitizing star names.
BLE: Updated code generation to reuse changes in stars.mk.

This Target produces and compiles a standalone C++ program for
a universe.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CompileTarget.h"

// Ptolemy kernel files
#include "miscFuncs.h"
#include "Scope.h"
#include "Galaxy.h"
#include "Target.h"
#include "KnownTarget.h"
#include "GalIter.h"
#include "Geodesic.h"
#include "ConstIters.h"

// Ptolemy domain includes
#include "AcyLoopScheduler.h"
#include "LoopScheduler.h"
#include "pt_fstream.h"
#include "SDFCluster.h"

// Standard includes
#include <ctype.h>			// define isalnum macro

// Constructor
CompileTarget::CompileTarget(const char* nam, const char* stype,
			     const char* desc, const char* assocDomain) :
HLLTarget(nam,stype,desc,assocDomain)
{
	destDirName = destDirectoryName(assocDomain);
	destDirectory.setInitValue(destDirName);
}

Block* CompileTarget::makeNew() const {
	LOG_NEW; return new CompileTarget(name(), starType(), descriptor(),
					  getAssociatedDomain());
}

void CompileTarget::setup() {
	CGTarget::chooseScheduler();

	// This kludge bypasses setup() in CGTarget, which casts
	// the portholes to CGPortHole.  These casts are no good for
	// this target, which has SDFPortHole types.
	Target::setup();
}

// do not call the begin methods
// this prevents Tcl/Tk windows from popping up twice
void CompileTarget::begin() {
}

int CompileTarget::writeGalDef(Galaxy& galaxy, StringList className) {
    // First generate the files that define the galaxies
    GalTopBlockIter next(galaxy);
    Block* b;
    while ((b = next++) != 0) {
	// Note the galaxy class name may not be a legal C++ identifier
	// since the class is really InterpGalaxy, and the className gets
	// set to equal the name of the galaxy, which is any Unix directory
	// name.
	if(!b->isItAtomic())
		writeGalDef(b->asGalaxy(), sanitize(b->className()));
    }

    // Prepare the output file for writing
    // -- use the original class name, not the sanitized class name
    // -- destructor for pt_ofstream will close the file automatically
    StringList galFileName = galaxy.className();
    galFileName << ".h";
    StringList codeFileName = logFilePathName(destDirectory, galFileName);
    pt_ofstream codeFile(codeFileName);
    if (!codeFile) return FALSE;

    myCode.initialize();
    myCode << "// Galaxy definition file from: " << className << "\n\n";
    myCode << "#ifndef _" << className << "_h\n#define _" << className <<
	    "_h 1\n";
    myCode << galDef(&galaxy, className, 1);
    myCode << "\n#endif\n";
    codeFile << myCode;
    codeFile.flush();
    return TRUE;
}

int CompileTarget::run() {

    // Call haltRequested to process pending events
    if (SimControl::haltRequested() || !galaxy()) {
	Error::abortRun("The compile-SDF target has no galaxy to run");
	return FALSE;
    }

    StringList universeClassName = sanitizedName(*galaxy());
    universeClassName << "Class";
    StringList universeName = sanitizedName(*galaxy());

    // First generate the files that define the galaxies
    GalTopBlockIter next(*galaxy());
    Block* b;
    while ((b = next++) != 0) {
	if(!b->isItAtomic())
		writeGalDef(b->asGalaxy(), sanitize(b->className()));
    }

    // Generate the C++ code file
    myCode.initialize();
    myCode += "// C++ code file generated from universe: ";
    myCode += galaxy()->fullName();

    myCode += "\n\nstatic const char file_id[] = \"code.cc\";\n\n";

    myCode += "char DEFAULT_DOMAIN[] = \"SDF\";";
    myCode += "// INCLUDE FILES\n";
    myCode += "#include \"CompiledUniverse.h\"\n";
    myCode += "#include \"GalIter.h\"\n";
    myCode += tcltkSetup();
    myCode += galDef(galaxy(), universeClassName, 0);
    myCode += "\n// MAIN FUNCTION\n";
    myCode += "main(int argc, char** argv) {\n";

    myCode += tcltkInitialize(universeName);

    myCode += "\n// Define the universe being run\n";
    myCode += universeClassName;
    myCode += " ";
    myCode += universeName;
    myCode += ";\n";
    myCode += universeName;
    myCode += ".setBlock(\"";
    myCode += universeName;
    myCode += "\");\n";
    myCode += "\n";
    myCode += "// Set the number of iterations\n";
    myCode += "int iterations = ";
    myCode += int(scheduler()->getStopTime());
    myCode += ";\n";

    myCode += universeName;
    myCode += ".parseCommandLine(argc, argv, &iterations);\n";

    myCode += "\n// INITIALIZE CODE\n";
    myCode += universeName;
    myCode += ".initialize();\n";

    myCode += "\n// BEGIN CODE\n";
    myCode += "{\n";
    myCode += "GalStarIter nextStar(";
    myCode += universeName;
    myCode += ");\n";
    myCode += "Star *s;\n";
    myCode += "while ((s = nextStar++) != 0) s->begin();\n";
    myCode += "if (SimControl::haltRequested()) return;\n";
    myCode += "}\n";

    myCode += "\n// MAIN LOOP\n";
    myCode += "while (iterations-- > 0) {\n";
    scheduler()->compileRun();
    myCode += "}\n";

    myCode += "\n// WRAPUP CODE\n";
    myCode += universeName;
    myCode += ".wrapup();\n";

    myCode += "exit(0);\n";
    myCode += "}\n";
    return TRUE;
}

void CompileTarget::wrapup() {
    if (Scheduler::haltRequested()) return;

    // Open code.cc
    StringList codeFileName = logFilePathName(destDirectory, "code.cc");
    pt_ofstream codeFile(codeFileName);
    if (!codeFile) return;

    // Write code to code.cc
    codeFile << myCode;
    codeFile.flush();

    // Check to see whether makefile is present, and if not, copy it in.
    // The makefile will invoke the compiler
    StringList cmd = "cd ";
    cmd += (const char*)destDirectory;
    cmd += "; if (test -r make.template) then ";
    cmd += "( echo make.template already exists) ";
    cmd += " else ";
    cmd += "( cp ";
    char* templName = expandPathName("$PTOLEMY/lib/CompileMake.template");
    cmd += templName;
    cmd += " make.template ) ";
    cmd += "fi";
    if(system(cmd)) {
	Error::abortRun("SDF compile target failed to copy ", templName,
			" into make.template");
        delete [] templName;
	return;
    }
    delete [] templName;

    // Invoke make depend
    cmd = "cd ";
    cmd += destDirectory;
    cmd += "; make -f make.template depend";
    system(cmd);
    cmd = "cd ";
    cmd += destDirectory;
    cmd += "; make";
    if(system(cmd)) {
	Error::abortRun("Compilation errors in code generated by the SDF compile target.");
	return;
    }

    cmd = "cd ";
    cmd += (const char*)destDirectory;
    cmd += "; mv -f code ";
    cmd += galaxy()->name();
    cmd += "; cp code.cc ";
    cmd += galaxy()->name();
    cmd += ".cc; ";
    cmd += "rm -f code.o; ";
    cmd += "strip ";
    cmd += galaxy()->name();
    cmd += "; ";
    cmd += galaxy()->name();
    system(cmd);
}

// Make the string origstr a valid C++ data structure specification
// by replacing characters that are neither alpha-numeric nor a period
// with an underscore character.  Mostly duplication of ptSanitize.  -BLE
static StringList mySanitize(const char *origstr) {
    char* cleanstr = savestring(origstr);
    char* strp = cleanstr;
    while ( *strp ) {
	if ( !isalnum(*strp) && *strp != '.' ) *strp = '_';
	strp++;
    }
    StringList out = cleanstr;
    delete [] cleanstr;
    return out;
}

// sanitizedStarName
// Generate the C++ version of a star name.
// If we use sanitizedFullName(s), then we get code such as
// sinMod_singen1_Ramp1 instead of sinMod.singen1.Ramp1.
// So, we sanitize each field in the star name and leave the periods
// between the fields since the periods access data members in
// the universe class.  Mostly duplication of fullName method.  -BLE
StringList CompileTarget::sanitizedStarName(const Star& s) const {
    StringList starName;
    if ( s.scope() ) {
	starName << mySanitize(s.scope()->fullName()) << ".";
    }
    if ( s.parent() ) {
	starName << mySanitize(s.parent()->fullName()) << ".";
    }
    starName << sanitize(s.name());
    return starName;
}

// Generate code for this star firing (usually from a schedule)
void CompileTarget::writeFiring(Star& s, int depth) {
    myCode << indent(depth) << sanitizedStarName(s) << ".run();\n";
}

// Method to return a pointer to the MultiPortHole that spawned a
// given PortHole, if there is such a thing.  If not, the pointer
// to the PortHole is returned as pointer to a GenericPort.
const GenericPort* CompileTarget::findMasterPort(const PortHole* p) const {
	const GenericPort* g = p->getMyMultiPortHole();
	if (!g) g = p;
	return g;
}

StringList CompileTarget::expandedName(const GenericPort* p) const {
	StringList out;
	if(p->isItMulti()) {
		out = sanitizedName(*p);
	} else {
		MultiPortHole* mph =
			((const PortHole *)p)->getMyMultiPortHole();
		if (mph) {
			out = mph->name();
			out += ".newPort()";
		} else {
			out = p->name();
		}
	}
	return out;
}

// Replace all quotation marks in a string with \"
StringList CompileTarget::quoteQuotationMarks(const char* str) {
    if (!str) return "";
    StringList ret;
    while (*str) {
	char piece[128];
	char* piecep = piece;
        for (int i = 0; i < 127; i++) {
	    if (*str == '\0') {
		*(piecep++) = '\0';
		break;
	    }
	    else if (*str == '\"') {
	        *(piecep++) = '\\';
	        *(piecep++) = '\"';
		i++;
	    }
	    else {
	        *(piecep++) = *str;
	    }
	    str++;
	}
	piece[127] = 0;			// make sure piece is null terminated
	ret << piece;
    }
    return ret;
}

// Define the routines necessary for Tcl/Tk (Wan-Teh Chang and Brian Evans)
StringList CompileTarget::tcltkSetup() {
    StringList myCode = "\n// Include files needed by Tcl/Tk commands\n";
    myCode += "#include <iostream.h>\n";
    myCode += "#include \"SimControl.h\"\n";
    myCode += "extern \"C\" {\n";
    myCode += "#include \"ptk.h\"\n";
    myCode += "#include \"itcl.h\"\n";
    myCode += "#include \"itk.h\"\n";
    myCode += "}\n";
    myCode += "\n// ptk.h includes tk.h which defines Complex to be 0\n";
    myCode += "#ifdef Complex\n";
    myCode += "#undef Complex\n";
    myCode += "#endif\n";
    myCode += "\n";
    myCode += "// Tcl/Tk commands\n";
    myCode += "// halt command -- ptkStop defined as synonym\n";
    myCode += "// we comment out unused parameters to prevent warnings\n";
    myCode += "static int halt_Cmd(ClientData /*clientData*/,\n";
    myCode += "        Tcl_Interp* /*interp*/,\n";
    myCode += "        int /*argc*/,\n";
    myCode += "        char** /*argv*/)\n";
    myCode += "{\n";
    myCode += "    SimControl::requestHalt();\n";
    myCode += "    return TCL_OK;\n";
    myCode += "}\n\n";

    return myCode;
}

// Initialize the Tcl/Tk interpreter (Wan-Teh Chang and Brian Evans)
StringList CompileTarget::tcltkInitialize(StringList& universeName) {
    StringList myCode = "\n// Initialize the Tcl/Tk interpreter\n";
    myCode += "ptkInterp = Tcl_CreateInterp();\n";
    myCode += "#if TK_MAJOR_VERSION <= 4 && TK_MINOR_VERSION < 1\n";
    myCode += "ptkW = Tk_CreateMainWindow(ptkInterp, NULL, \"";
    myCode += universeName;
    myCode += "\", \"Pigi\");\n";
    myCode += "#endif /* TK_MAJOR_VERSION <= 4 && TK_MINOR_VERSION < 1 */\n";
    myCode +=
"if (Tcl_Init(ptkInterp) == TCL_ERROR) {\n"
"    cerr << \"Tcl_Init: Error initializing the Tcl interpreter\n\" <<\n"
"      Tcl_GetVar(ptkInterp, \"errorInfo\", TCL_GLOBAL_ONLY) << \"\n\";\n"
"    exit(1);\n"
"}\n";

    myCode +=
"#if (TK_MAJOR_VERSION >= 4 && TK_MINOR_VERSION >= 1) || TK_MAJOR_VERSION >= 8\n"
"ptkW = Tk_MainWindow(ptkInterp);\n"
"#endif /* (TK_MAJOR_VERSION >= 4 && TK_MINOR_VERSION >= 1) || TK_MAJOR_VERSION >= 8 */\n";

    myCode +=
"\n"
"// Define halt and ptkStop Tcl commands, and initialize Tk\n"
"Tcl_CreateCommand(ptkInterp, \"halt\", halt_Cmd, 0, 0);\n"
"Tcl_CreateCommand(ptkInterp, \"ptkStop\", halt_Cmd, 0, 0);\n"
"if (Tk_Init(ptkInterp) == TCL_ERROR) {\n"
"    cerr << \"Tk_Init: Error initializing the Tk interpreter\n\" <<\n"
"      Tcl_GetVar(ptkInterp, \"errorInfo\", TCL_GLOBAL_ONLY) << \"\n\";\n"
"    exit(1);\n"
"}\n";

    myCode +=
"if (Itcl_Init(ptkInterp) == TCL_ERROR) {\n"
"    cerr << \"Tk_Init: Error initializing the Itcl interpreter\n\" <<\n"
"      Tcl_GetVar(ptkInterp, \"errorInfo\", TCL_GLOBAL_ONLY) << \"\n\";\n"
"    exit(1);\n"
"}\n";

    myCode +=
"if (Itk_Init(ptkInterp) == TCL_ERROR) {\n"
"    cerr << \"Tk_Init: Error initializing the Itk interpreter\n\" <<\n"
"      Tcl_GetVar(ptkInterp, \"errorInfo\", TCL_GLOBAL_ONLY) << \"\n\";\n"
"    exit(1);\n"
"}\n";

    myCode +=
"\n"
"// Hide the wish window\n"
"Tcl_Eval(ptkInterp, \"wm withdraw .\");\n";

    myCode +=
"\n"
"// Read pigi tcl initialization files to set key bindings, colors, etc.\n"
"char *fulldirname = expandPathName(\"$PTOLEMY/lib/tcl/pigilib.tcl\");\n"
"if (Tcl_EvalFile(ptkInterp, fulldirname) != TCL_OK) {\n"
"    cerr << \"Tcl_EvalFile: Error in evaluating \" <<\n"
"      fulldirname << \"\n\" << \n"
"      Tcl_GetVar(ptkInterp, \"errorInfo\", TCL_GLOBAL_ONLY) << \"\n\";\n"
"    exit(1);\n"
"}\n"
"delete [] fulldirname;\n";

    myCode += "\n// Some Tcl/Tk stars use these ptcl commands.\n";
    myCode += "Tcl_Eval(ptkInterp, \"proc curuniverse {} { return ";
    myCode += universeName;
    myCode += " }\");\n";
    myCode += "extern int runEventsOnTimer();\n";
    myCode += "SimControl::setPollAction(runEventsOnTimer);\n";

    myCode += "\n// Mimic a Ptolemy run control panel\n";
    myCode += "Tcl_Eval(ptkInterp, \"ptkRunControlStandalone ";
    myCode += universeName;
    myCode += "\");\n";

    return myCode;
}

// Define a galaxy
StringList CompileTarget::galDef(Galaxy* galaxy,
			StringList className, int level) {
    StringList myCode;
    myCode.initialize();

    // The following iterator looks only at the current level of the graph
    GalTopBlockIter next(*galaxy);
    Block* b;
    while ((b = next++) != 0) {
	// An include file is generated for every block.  This means some
	// classes will have their definitions included more than once.
	// This is harmless.
	myCode << "#include \"" << b->className() << ".h\"\n";
    }
    // Generate include statements for galaxy states
    BlockStateIter galStateIter(*galaxy);
    State* galState;
    while ((galState = galStateIter++) != 0) {
	myCode += "#include \"";
	myCode += galState->className();
	myCode += ".h\"\n";
    }

    myCode += "\n";
    myCode += "// GALAXY DECLARATION\n";
    myCode += "class ";
    myCode += className;
    if (level == 0) myCode += " : public CompiledUniverse {\npublic:\n";
    else myCode += " : public Galaxy {\npublic:\n";
    next.reset();
    while ((b = next++) != 0) {
	// Declare the stars and galaxies used
	myCode += indent(1);
	// Have to sanitize below because the class may be an InterpGalaxy
	myCode += sanitize(b->className());
	myCode += " ";
	myCode += sanitizedName(*b);
	myCode += ";\n";
    }
    // Generate galaxy port declarations, MultiPortHoles first
    myCode += "\n// PortHole declarations\n";
    StringList galSetPorts = "";
    StringList portAliases = "";
    CBlockMPHIter galMPHIter(*galaxy);
    const MultiPortHole* mph;
    while ((mph = galMPHIter++) != 0) {
	myCode += indent(1);
	myCode += "GalMultiPort ";
	myCode += sanitizedName(*mph);
	myCode += ";\n";
	galSetPorts += indent(1);
	galSetPorts += sanitizedName(*mph);
	galSetPorts += ".setPort(\"";
	galSetPorts += sanitizedName(*mph);
	galSetPorts += "\", this, ";
	galSetPorts += mph->type();
	galSetPorts += ");\n";
	// generate the alias
	// We should go down one level of aliasing only
	GenericPort* mpha = mph->alias();
	portAliases += indent(1);
	portAliases += "alias(";
	portAliases += sanitizedName(*mph);
	portAliases += ", ";
	portAliases += sanitizedName(*(mpha->parent()));
	portAliases += ".";
	portAliases += sanitizedName(*mpha);
	portAliases += ");\n";
    }
    CBlockPortIter galPortIter(*galaxy);
    const PortHole* ph;
    while ((ph = galPortIter++) != 0) {
	// Check to see whether it's an instance of a MPH
	if(!ph->getMyMultiPortHole()) {
	    myCode += indent(1);
	    myCode += "GalPort ";
	    myCode += sanitizedName(*ph);
	    myCode += ";\n";
	    galSetPorts += indent(1);
	    galSetPorts += sanitizedName(*ph);
	    galSetPorts += ".setPort(\"";
	    galSetPorts += sanitizedName(*ph);
	    galSetPorts += "\", this, ";
	    galSetPorts += ph->type();
	    galSetPorts += ");\n";
	    // generate the alias
	    // We should go down one level of aliasing only
	    // The cast is OK because the alias of a PortHole is always PortHole
	    const GenericPort* pha = ph->alias();
	    portAliases += indent(1);
	    portAliases += "alias(";
	    portAliases += sanitizedName(*ph);
	    portAliases += ", ";
	    portAliases += sanitizedName(*(ph->alias()->parent()));
	    portAliases += ".";
	    portAliases += expandedName(pha);
	    portAliases += ");\n";
	}
    }
    // Generate galaxy states declarations
    myCode += "\n// state declarations\n";
    galStateIter.reset();
    while ((galState = galStateIter++) != 0) {
	myCode += indent(1);
	myCode += galState->className();
	myCode += " ";
	myCode += sanitizedName(*galState);
	myCode += ";\n";
    }
    myCode += "\n// constructor\n";
    myCode += indent(1);
    myCode += className;
    myCode += " ();\n";
    myCode += "};\n";
    myCode += "\n// TOPOLOGY DEFINITION\n";
    myCode += className;
    myCode += " :: ";
    myCode += className;
    myCode += " () {\n";
    // Set universe states
    galStateIter.reset();
    while ((galState = galStateIter++) != 0) {
	myCode += indent(1);
	myCode += "addState(";
	myCode += sanitizedName(*galState);
	myCode += ".setState(\"";
	myCode += galState->name();
	myCode += "\", this, \"";
	myCode += quoteQuotationMarks(galState->initValue());
	myCode += "\"));\n";
    }
    next.reset();
    while ((b = next++) != 0) {
	// Add the stars and galaxies used to the knownblock list
	myCode += indent(1);
	myCode += "addBlock(";
	myCode += sanitizedName(*b);
	myCode += ",\"";
	myCode += sanitizedName(*b);
	myCode += "\");\n";
    }
    myCode += galSetPorts;
    myCode += "\n";
    myCode += indent(1);
    myCode += "// set states\n";
    next.reset();
    while ((b = next++) != 0) {
	BlockStateIter nextState(*b);
	const State* s;
	while ((s = nextState++) != 0) {
	    myCode += indent(1);
	    myCode += sanitizedName(*b);
	    myCode += ".setState(\"";
	    myCode += s->name();
	    myCode += "\",\"";
	    // Want to get initial value here -- before processing
	    myCode += quoteQuotationMarks(s->initValue());
	    myCode += "\");\n";
	}
    }

    myCode += "\n";
    myCode += indent(1);
    myCode += "// make connections\n";

    // Start with the aliases to galaxy input ports
    myCode += portAliases;

    next.reset();
    while ((b = next++) != 0) {
	// Work with blocks one at time
	BlockPortIter nextPort(*b);
	const PortHole* p;
	PortHole* farPort;
	const GenericPort *g;
	while ((p = nextPort++) != 0) {
	    // Make connections only for output ports.
	    if (p->isItOutput()) {
		// If this port was spawned by a MultiPortHole, get a pointer
		// to that MultiPortHole
		g = findMasterPort(p);
		// Check to see whether the port is aliased to a galaxy port
		if(!g->aliasFrom()) {
		    myCode += indent(1);
		    myCode += "connect(";
		    myCode += sanitizedName(*b);
		    myCode += ".";
		    myCode += expandedName(p);
		    myCode += ", ";
		    if (!(farPort = p->far())) {
			// What we have is the output port of a galaxy, meaning
			// there is an extra step to find its destination
			// Following cast should be safe because the far side
			// should always be a PortHole, not a MultiPortHole.
			farPort = ((PortHole&)p->realPort()).far();
		    }
		    if (!farPort) {
			Error::abortRun(b->fullName(),
				" disconnected Universe.");
			return 0;
		    }
		    // Work up to top level of aliases
		    GenericPort* gp = farPort;
		    while(gp->aliasFrom()) { gp = gp->aliasFrom(); }
		    myCode += sanitizedName(*(gp->parent()));
		    myCode += ".";
		    // FIXME: If the gp port is a multiporthole,
		    // there is no guarantee here that it will be
		    // wired up in the same order as in the original graph
		    // under the SDF-default target.
		    myCode += expandedName(gp);
		    myCode += ", ";
		    myCode += ((PortHole&)p->realPort()).numInitDelays();
		    myCode += ", \"";
 		    myCode += ((PortHole&)p->realPort()).initDelayValues();
		    myCode += "\");\n";
		}
	    }
	}
    }
    myCode += "}\n";
    return myCode;
}

static CompileTarget compileTargetProto("compile-SDF", "SDFStar",
	"Generate and compile standalone C++ code");
static KnownTarget entry(compileTargetProto, "compile-SDF");
