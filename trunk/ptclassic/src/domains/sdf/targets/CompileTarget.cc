static const char file_id[] = "CompileTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 12/8/91

This Target produces and compiles a standalone C++ program for
a universe.

*******************************************************************/

#include "BaseCTarget.h"
#include "KnownTarget.h"
#include "UserOutput.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "Geodesic.h"
#include "ConstIters.h"
#include <ctype.h>

class CompileTarget : public BaseCTarget {
public:
	int setup(Galaxy& g);
	int run();
	void wrapup ();
	Block* clone() const
		{ LOG_NEW; return new CompileTarget(*this);}

	// Routines for writing code: schedulers may call these
	StringList writeFiring(Star& s, int depth);

private:
	// Method to return a pointer to the MultiPortHole that spawned a
	// given PortHole, if there is such a thing.  If not, the pointer
	// to the PortHole is returned as pointer to a GenericPort.
	GenericPort* findMasterPort(const PortHole* p) const;

	// Returns the name of an ordinary porthole, or
	// "name.newPort()" for a MultiPortHole.
	StringList expandedName(const GenericPort* p) const;

	StringList galDef(Galaxy* galaxy, StringList className, int level);

	int writeGalDef(Galaxy& galaxy, const StringList className);

};

int CompileTarget::setup(Galaxy& g) {
	// This kludge bypasses setup() in CGTarget, which casts
	// the portholes to CGPortHole.  These casts are no good for
	// this target, which has SDFPortHole types.
	return Target::setup(g);
}

int CompileTarget::writeGalDef(Galaxy& galaxy, const StringList className) {
    // First generate the files that define the galaxies
    GalTopBlockIter next(galaxy);
    Block* b;
    while ((b = next++) != 0) {
	// Note the galaxy class name may not be a legal C++ identifier
	// since the class is really InterpGalaxy, and the className gets
	// set to equal the name of the galaxy, which is any Unix directory
	// name.
	if(!b->isItAtomic())
		writeGalDef(b->asGalaxy(), sanitize(b->readClassName()));
    }

    // prepare the output file for writing
    // Use the original class name, not the sanitized class name
    StringList galFileName = galaxy.readClassName();
    galFileName += ".h";
    char* codeFileName = writeFileName(galFileName);
    UserOutput codeFile;
    if(!codeFile.fileName(codeFileName)) {
	Error::abortRun("Can't open code file for writing: ",codeFileName);
	return FALSE;
    }

    StringList runCode = "// Galaxy definition file from: ";
    runCode += className;
    runCode += "\n";
    runCode += "\n";
    runCode += "#ifndef _";
    runCode += className;
    runCode += "_h\n#define _";
    runCode += className;
    runCode += "_h 1\n";
    runCode += galDef(&galaxy, className, 1);
    runCode += "\n#endif\n";
    codeFile << runCode;
    codeFile.flush();
    return TRUE;
}

int CompileTarget::run() {
    StringList universeClassName = sanitizedName(*gal);
    universeClassName += "Class";
    StringList universeName = sanitizedName(*gal);

    // First generate the files that define the galaxies
    GalTopBlockIter next(*gal);
    Block* b;
    while ((b = next++) != 0) {
	if(!b->isItAtomic())
		writeGalDef(b->asGalaxy(), sanitize(b->readClassName()));
    }

    // Generate the C++ code file
    StringList runCode = "// C++ code file generated from universe: ";
    runCode += gal->readFullName();

    runCode += "\n\nstatic const char file_id[] = \"code.cc\";\n\n";

    runCode += "// INCLUDE FILES\n";
    runCode += "#include \"CompiledUniverse.h\"\n";
    runCode += galDef(gal, universeClassName, 0);

    runCode += "\n";
    runCode += "// MAIN FUNCTION\n";
    runCode += "main(int argc, char** argv) {\n";
    runCode += "int iterations;\n";
    runCode += universeClassName;
    runCode += " ";
    runCode += universeName;
    runCode += ";\n";
    runCode += universeName;
    runCode += ".setBlock(\"";
    runCode += universeName;
    runCode += "\");\n";
    runCode += "\n";
    // Note that the following will only work for SDF schedulers
    // It will crash ungracefully for anything else
    runCode += "// set default value for number of iterations\n";
    runCode += "iterations = ";
    runCode += ((SDFScheduler*)mySched())->getStopTime();
    runCode += ";\n";

    runCode += universeName;
    runCode += ".parseCommandLine(argc, argv, &iterations);\n";

    runCode += "\n// INITIALIZE CODE\n";
    runCode += universeName;
    runCode += ".initialize();\n";

    runCode += "\n";
    runCode += "// MAIN LOOP\n";
    runCode += "while(iterations-- > 0) {\n";
    runCode += mySched()->compileRun();
    runCode += "}\n";

    runCode += "// WRAPUP CODE\n";
    runCode += universeName;
    runCode += ".wrapup();\n";

    runCode += "exit(0);\n";
    runCode += "}\n";
    addCode(runCode);
    return 1;
}

void CompileTarget::wrapup() {
    char* codeFileName = writeFileName("code.cc");
    UserOutput codeFile;
    if(Scheduler::haltRequested()) return;
    if(!codeFile.fileName(codeFileName)) {
	Error::abortRun("Can't open code file for writing: ",codeFileName);
	return;
    }
    writeCode(codeFile);

    // Invoke the compiler
    StringList cmd;
    // Check to see whether makefile is present, and if not, copy it in.

    cmd = "cd ";
    cmd += destDirectory;
    cmd += "; if (test -r make.template) then ";
    cmd += "( echo make.template already exists) ";
    cmd += " else ";
    cmd += "( cp ";
    const char* templName = expandPathName("~ptolemy/lib/CompileMake.template");
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
    cmd += destDirectory;
    cmd += "; mv -f code ";
    cmd += gal->readName();
    cmd += "; cp code.cc ";
    cmd += gal->readName();
    cmd += ".cc; ";
    cmd += "rm -f code.o; ";
    cmd += "strip ";
    cmd += gal->readName();
    cmd += "; ";
    cmd += gal->readName();
    system(cmd);
}


StringList CompileTarget::writeFiring(Star& s, int depth) {
	StringList out;
	out = indent(depth);
	out += sanitizedFullName(s);
	out += ".fire();\n";
	return out;
}

GenericPort* CompileTarget::findMasterPort(const PortHole* p) const {
	GenericPort* g;
	if(!(g = (GenericPort*) p->getMyMultiPortHole()))
		g = (GenericPort*) p;
	return g;
}

StringList CompileTarget::expandedName(const GenericPort* p) const {
    StringList out;
    if(p->isItMulti()) {
	out = sanitizedName(*p);
    } else {
	MultiPortHole* g;
	if(g = ((PortHole*)p)->getMyMultiPortHole()) {
		out = g->readName();
		out += ".newPort()";
	} else {
		out = p->readName();
	}
    }
    return out;
}

// Define a galaxy
StringList CompileTarget::galDef(Galaxy* galaxy,
			StringList className, int level) {
    StringList runCode = "";
    // The following iterator looks only at the current level of the graph
    GalTopBlockIter next(*galaxy);
    Block* b;
    while ((b = next++) != 0) {
	// An include file is generated for every block.  This means some
	// classes will have their definitions included more than once.
	// This is harmless.
	runCode += "#include \"";
	runCode += b->readClassName();
	runCode += ".h\"\n";
    }
    // Generate include statements for galaxy states
    BlockStateIter galStateIter(*galaxy);
    State* galState;
    while ((galState = galStateIter++) != 0) {
	runCode += "#include \"";
	runCode += galState->readClassName();
	runCode += ".h\"\n";
    }

    runCode += "\n";
    runCode += "// GALAXY DECLARATION\n";
    runCode += "class ";
    runCode += className;
    if(level==0) runCode += " : public CompiledUniverse {\npublic:\n";
    else runCode += " : public Galaxy {\npublic:\n";
    next.reset();
    while ((b = next++) != 0) {
	// Declare the stars and galaxies used
	runCode += indent(1);
	// Have to sanitize below because the class may be an InterpGalaxy
	runCode += sanitize(b->readClassName());
	runCode += " ";
	runCode += sanitizedName(*b);
	runCode += ";\n";
    }
    // Generate galaxy port declarations, MultiPortHoles first
    runCode += "\n// PortHole declarations\n";
    StringList galSetPorts = "";
    StringList portAliases = "";
    CBlockMPHIter galMPHIter(*galaxy);
    const MultiPortHole* mph;
    while ((mph = galMPHIter++) != 0) {
	runCode += indent(1);
	runCode += "GalMultiPort ";
	runCode += sanitizedName(*mph);
	runCode += ";\n";
	galSetPorts += indent(1);
	galSetPorts += sanitizedName(*mph);
	galSetPorts += ".setPort(\"";
	galSetPorts += sanitizedName(*mph);
	galSetPorts += "\", this, ";
	galSetPorts += mph->myType();
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
	    runCode += indent(1);
	    runCode += "GalPort ";
	    runCode += sanitizedName(*ph);
	    runCode += ";\n";
	    galSetPorts += indent(1);
	    galSetPorts += sanitizedName(*ph);
	    galSetPorts += ".setPort(\"";
	    galSetPorts += sanitizedName(*ph);
	    galSetPorts += "\", this, ";
	    galSetPorts += ph->myType();
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
    runCode += "\n// state declarations\n";
    galStateIter.reset();
    while ((galState = galStateIter++) != 0) {
	runCode += indent(1);
	runCode += galState->readClassName();
	runCode += " ";
	runCode += sanitizedName(*galState);
	runCode += ";\n";
    }
    runCode += "\n// constructor\n";
    runCode += indent(1);
    runCode += className;
    runCode += " ();\n";
    runCode += "};\n";
    runCode += "\n// TOPOLOGY DEFINITION\n";
    runCode += className;
    runCode += " :: ";
    runCode += className;
    runCode += " () {\n";
    // Set universe states
    galStateIter.reset();
    while ((galState = galStateIter++) != 0) {
	runCode += indent(1);
	runCode += "addState(";
	runCode += sanitizedName(*galState);
	runCode += ".setState(\"";
	runCode += galState->readName();
	runCode += "\", this, \"";
	runCode += galState->getInitValue();
	runCode += "\"));\n";
    }
    next.reset();
    while ((b = next++) != 0) {
	// Add the stars and galaxies used to the knownblock list
	runCode += indent(1);
	runCode += "addBlock(";
	runCode += sanitizedName(*b);
	runCode += ",\"";
	runCode += sanitizedName(*b);
	runCode += "\");\n";
    }
    runCode += galSetPorts;
    runCode += "\n";
    runCode += indent(1);
    runCode += "// set states\n";
    next.reset();
    while ((b = next++) != 0) {
	BlockStateIter nextState(*b);
	const State* s;
	while ((s = nextState++) != 0) {
	    runCode += indent(1);
	    runCode += sanitizedName(*b);
	    runCode += ".setState(\"";
	    runCode += s->readName();
	    runCode += "\",\"";
	    // Want to get initial value here -- before processing
	    runCode += s->getInitValue();
	    runCode += "\");\n";
	}
    }

    runCode += "\n";
    runCode += indent(1);
    runCode += "// make connections\n";

    // Start with the aliases to galaxy input ports
    runCode += portAliases;

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
		    runCode += indent(1);
		    runCode += "connect(";
		    runCode += sanitizedName(*b);
		    runCode += ".";
		    runCode += expandedName(p);
		    runCode += ", ";
		    if (!(farPort = p->far())) {
			// What we have is the output port of a galaxy, meaning
			// there is an extra step to find its destination
			// Following cast should be safe because the far side
			// should always be a PortHole, not a MultiPortHole.
			farPort = ((PortHole&)p->realPort()).far();
		    }
		    if (!farPort) {
			Error::abortRun(b->readFullName(),
				" Disconnected Universe.");
			addCode(runCode);
			return 0;
		    }
		    // Work up to top level of aliases
		    GenericPort* gp = farPort;
		    while(gp->aliasFrom()) { gp = gp->aliasFrom(); }
		    runCode += sanitizedName(*(gp->parent()));
		    runCode += ".";
		    runCode += expandedName(gp);
		    runCode += ", ";
		    runCode += ((PortHole&)p->realPort()).myGeodesic->numInit();
		    runCode += ");\n";
		}
	    }
	}
    }
    runCode += "}\n";
    return runCode;
}

static CompileTarget compileTargetProto("compile-SDF", "SDFStar",
	"Generate and compile stanalone C++ code");
static KnownTarget entry(compileTargetProto,"compile-SDF");

