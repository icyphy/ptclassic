static const char file_id[] = "CompileTarget.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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

 Programmer:  E. A. Lee
 Date of creation: 12/8/91

This Target produces and compiles a standalone C++ program for
a universe.

*******************************************************************/
#ifdef __GNUG__
#pragma implementation
#endif

#include "CompileTarget.h"
#include "LoopScheduler.h"
#include "KnownTarget.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "Geodesic.h"
#include "ConstIters.h"
#include <ctype.h>
#include "pt_fstream.h"
#include "SDFCluster.h"

Block* CompileTarget::makeNew() const {
	LOG_NEW; return new CompileTarget(name(), starType(), descriptor());
}

void CompileTarget::setup() {
	char* schedFileName = 0;
	writeDirectoryName(destDirectory);
	int lv = int(loopingLevel);
	if (lv) {
		schedFileName = writeFileName("schedule.log");
		if (lv == 1) {
			LOG_NEW; setSched(new SDFClustSched(schedFileName));
		} else {
			LOG_NEW; setSched(new LoopScheduler(schedFileName));
		}
	}
	else {
		LOG_NEW; setSched(new SDFScheduler);
	}
	// This kludge bypasses setup() in CGTarget, which casts
	// the portholes to CGPortHole.  These casts are no good for
	// this target, which has SDFPortHole types.
	Target::setup();
	LOG_DEL; delete [] schedFileName;
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

    // prepare the output file for writing
    // Use the original class name, not the sanitized class name
    StringList galFileName = galaxy.className();
    galFileName += ".h";
    char* codeFileName = writeFileName(galFileName);
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
    StringList universeClassName = sanitizedName(*galaxy());
    universeClassName += "Class";
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

    myCode += "// INCLUDE FILES\n";
    myCode += "#include \"CompiledUniverse.h\"\n";
    myCode += galDef(galaxy(), universeClassName, 0);

    myCode += "\n";
    myCode += "// MAIN FUNCTION\n";
    myCode += "main(int argc, char** argv) {\n";
    myCode += "int iterations;\n";
    myCode += universeClassName;
    myCode += " ";
    myCode += universeName;
    myCode += ";\n";
    myCode += universeName;
    myCode += ".setBlock(\"";
    myCode += universeName;
    myCode += "\");\n";
    myCode += "\n";
    myCode += "// set default value for number of iterations\n";
    myCode += "iterations = ";
    myCode += scheduler()->getStopTime();
    myCode += ";\n";

    myCode += universeName;
    myCode += ".parseCommandLine(argc, argv, &iterations);\n";

    myCode += "\n// INITIALIZE CODE\n";
    myCode += universeName;
    myCode += ".initialize();\n";

    myCode += "\n";
    myCode += "// MAIN LOOP\n";
    myCode += "while(iterations-- > 0) {\n";
    scheduler()->compileRun();
    myCode += "}\n";

    myCode += "// WRAPUP CODE\n";
    myCode += universeName;
    myCode += ".wrapup();\n";

    myCode += "exit(0);\n";
    myCode += "}\n";
    return 1;
}

void CompileTarget::wrapup() {
    char* codeFileName = writeFileName("code.cc");
    if(Scheduler::haltRequested()) return;
    pt_ofstream codeFile(codeFileName);
    if (!codeFile) return;
    codeFile << myCode;
    codeFile.flush();

    // Invoke the compiler
    StringList cmd;
    // Check to see whether makefile is present, and if not, copy it in.

    cmd = "cd ";
    cmd += (const char*)destDirectory;
    cmd += "; if (test -r make.template) then ";
    cmd += "( echo make.template already exists) ";
    cmd += " else ";
    cmd += "( cp ";
    const char* templName = expandPathName("$PTOLEMY/lib/CompileMake.template");
    cmd += templName;
    cmd += " make.template ) ";
    cmd += "fi";
    if(system(cmd)) {
	Error::abortRun("Failed to copy ", templName, " into make.template");
	return;
    }
    // Invoke make depend
    cmd = "cd ";
    cmd += destDirectory;
    cmd += "; make -f make.template depend";
    system(cmd);
    cmd = "cd ";
    cmd += destDirectory;
    cmd += "; make";
    if(system(cmd)) {
	Error::abortRun("Compilation errors in generated code.");
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


void CompileTarget::writeFiring(Star& s, int depth) {
	myCode << indent(depth) << sanitizedFullName(s) << ".run();\n";
}

const GenericPort* CompileTarget::findMasterPort(const PortHole* p) const {
	const GenericPort* g = p->getMyMultiPortHole();
	if (!g) 
		g = p;
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
	myCode += "#include \"";
	myCode += b->className();
	myCode += ".h\"\n";
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
    if(level==0) myCode += " : public CompiledUniverse {\npublic:\n";
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
	myCode += galState->initValue();
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
	    myCode += s->initValue();
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
				" Disconnected Universe.");
			return 0;
		    }
		    // Work up to top level of aliases
		    GenericPort* gp = farPort;
		    while(gp->aliasFrom()) { gp = gp->aliasFrom(); }
		    myCode += sanitizedName(*(gp->parent()));
		    myCode += ".";
		    myCode += expandedName(gp);
		    myCode += ", ";
		    myCode += ((PortHole&)p->realPort()).geo()->numInit();
		    myCode += ");\n";
		}
	    }
	}
    }
    myCode += "}\n";
    return myCode;
}

static CompileTarget compileTargetProto("compile-SDF", "SDFStar",
	"Generate and compile stanalone C++ code");
static KnownTarget entry(compileTargetProto,"compile-SDF");

