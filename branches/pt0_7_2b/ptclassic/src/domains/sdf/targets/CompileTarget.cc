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

#include "CGTarget.h"
#include "KnownTarget.h"
#include "LoopScheduler.h"
#include "SDFScheduler.h"
#include "StringState.h"
#include "IntState.h"
#include "UserOutput.h"
#include "Galaxy.h"
#include "GalIter.h"
#include "Geodesic.h"
#include "ConstIters.h"

class CompileTarget : public CGTarget {
private:
	int id;
	// Method to return a pointer to the MultiPortHole that spawned a
	// given PortHole, if there is such a thing.  If not, the pointer
	// to the PortHole is returned as pointer to a GenericPort.
	GenericPort* findMasterPort(const PortHole* p) const;
	// Returns the sanitized name of an ordinary porthole, or
	// "name.newPort()" for a MultiPortHole.
	StringList expandedName(const GenericPort* p) const;
	StringList galDef(Galaxy* galaxy, StringList className, int level);
	int writeGalDef(Galaxy& galaxy, const char* className);
protected:
	StringState destDirectory;
	IntState loopScheduler;
	char *graphFileName, *clusterFileName, *schedFileName;
public:
	CompileTarget();
	void start();
	int run();
	void wrapup ();
	Block* clone() const { return &(new CompileTarget)->copyStates(*this);}

	// Routines for writing code: schedulers may call these
	StringList beginIteration(int repetitions, int depth);
	StringList endIteration(int repetitions, int depth);
	StringList writeFiring(Star& s, int depth);

	~CompileTarget() { delSched();}
};

// constructor
CompileTarget::CompileTarget() : CGTarget("compile-SDF","SDFStar",
"Generate stand-alone C++ programs and compile them.  The program\n"
"and associated makefile is written to a directory given as a Target param.\n"
"Can use either the default SDF scheduler or Shuvra's loop scheduler."),
graphFileName(0), clusterFileName(0), schedFileName(0), id(0)
{
	addState(destDirectory.setState("destDirectory",this,"PTOLEMY_SYSTEMS",
			"Directory to write to"));
	addState(loopScheduler.setState("loopScheduler",this,"NO",
			"Specify whether to use loop scheduler."));
}

void CompileTarget::start() {

    delete dirFullName;
    dirFullName = writeDirectoryName((char*)destDirectory);

    if(int(loopScheduler)) {
	char* schedFileName = writeFileName("schedule");
	char* graphFileName = writeFileName("expanded-graph");
	char* clusterFileName = writeFileName("cluster-graph");
	setSched(new
		LoopScheduler(schedFileName,graphFileName,clusterFileName));
    } else {
	setSched(new SDFScheduler);
    }
}

int CompileTarget::writeGalDef(Galaxy& galaxy, const char* className) {
    // First generate the files that define the galaxies
    GalTopBlockIter next(galaxy);
    Block* b;
    while ((b = next++) != 0) {
	if(!b->isItAtomic()) writeGalDef(b->asGalaxy(), b->readClassName());
    }

    // prepare the output file for writing
    StringList galFileName = className;
    galFileName += ".h";
    char* codeFileName = writeFileName(galFileName);
    UserOutput codeFile;
    if(!codeFile.fileName(codeFileName)) {
	Error::abortRun("Can't open code file for writing: ",codeFileName);
	return 0;
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
}

int CompileTarget::run() {
    StringList universeClassName = gal->sanitizedName();
    universeClassName += "Class";
    StringList universeName = gal->sanitizedName();

    // First generate the files that define the galaxies
    GalTopBlockIter next(*gal);
    Block* b;
    while ((b = next++) != 0) {
	if(!b->isItAtomic()) writeGalDef(b->asGalaxy(), b->readClassName());
    }

    // Generate the C++ code file
    StringList runCode = "// C++ code file generated from universe: ";
    runCode += gal->readFullName();
    runCode += "\n";

    runCode += "\n";
    runCode += "// INCLUDE FILES\n";
    runCode += "#include \"CompiledUniverse.cc\"\n";
    runCode += "#include \"CompiledError.cc\"\n";
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
    // NOTE: Because sh doesn't support tilde-expansion (no wonder csh came
    // along!) we are forced at assume ~ptolemy = /usr/users/ptolemy.
    // Caveat hacker.
    cmd = "cd ";
    cmd += (char*)destDirectory;
    cmd += "; if (test -r make.template) then ";
    cmd += "( echo make.template already exists) ";
    cmd += " else ";
    cmd += "( cp /usr/users/ptolemy/lib/CompileMake.template make.template ) ";
    cmd += "fi";
    if(system(cmd)) {
	Error::abortRun("Failed to copy ",
	   "/usr/users/ptolemy/lib/CompileMake.template into make.template");
	return;
    }
    // Invoke make depend
    cmd = "cd ";
    cmd += (char*)destDirectory;
    cmd += "; make -f make.template depend";
    system(cmd);
    cmd = "cd ";
    cmd += (char*)destDirectory;
    cmd += "; make";
    if(system(cmd)) {
	Error::abortRun("Compilation errors in generated code.");
	return;
    }
    cmd = "cd ";
    cmd += (char*)destDirectory;
    cmd += "; mv -f code ";
    cmd += gal->sanitizedName();
    cmd += "; cp code.cc ";
    cmd += gal->sanitizedName();
    cmd += ".cc; ";
    cmd += "rm -f code.o; ";
    cmd += "strip ";
    cmd += gal->sanitizedName();
    cmd += "; ";
    cmd += gal->sanitizedName();
    system(cmd);
}


// Routines for writing code: schedulers may call these
StringList CompileTarget::beginIteration(int repetitions, int depth) {
	StringList out;
	out = indent(depth);
	out += "int i";
	out += id;
	out += "; ";
	out += "for (i";
	out += id;
	out += "=0; i";
	out += id;
	out += " < ";
	out += repetitions;
	out += "; i";
	out += id++;
	out += "++) {\n";
	return out;
}

StringList CompileTarget::endIteration(int repetitions, int depth) {
	StringList out;
	out = indent(depth);
	out += "}\n";
	return out;
}

StringList CompileTarget::writeFiring(Star& s, int depth) {
	StringList out;
	out = indent(depth);
	out += s.sanitizedFullName();
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
	out = p->sanitizedName();
    } else {
	MultiPortHole* g;
	if(g = ((PortHole*)p)->getMyMultiPortHole()) {
		out = g->sanitizedName();
		out += ".newPort()";
	} else {
		out = p->sanitizedName();
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
	runCode += b->readClassName();
	runCode += " ";
	runCode += b->sanitizedName();
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
	runCode += mph->sanitizedName();
	runCode += ";\n";
	galSetPorts += indent(1);
	galSetPorts += mph->sanitizedName();
	galSetPorts += ".setPort(\"";
	galSetPorts += mph->sanitizedName();
	galSetPorts += "\", this, ";
	galSetPorts += mph->myType();
	galSetPorts += ");\n";
	// generate the alias
	// We should go down one level of aliasing only
	GenericPort* mpha = mph->alias();
	portAliases += indent(1);
	portAliases += "alias(";
	portAliases += mph->sanitizedName();
	portAliases += ", ";
	portAliases += mpha->parent()->sanitizedName();
	portAliases += ".";
	portAliases += mpha->sanitizedName();
	portAliases += ");\n";
    }
    CBlockPortIter galPortIter(*galaxy);
    const PortHole* ph;
    while ((ph = galPortIter++) != 0) {
	// Check to see whether it's an instance of a MPH
	if(!ph->getMyMultiPortHole()) {
	    runCode += indent(1);
	    runCode += "GalPort ";
	    runCode += ph->sanitizedName();
	    runCode += ";\n";
	    galSetPorts += indent(1);
	    galSetPorts += ph->sanitizedName();
	    galSetPorts += ".setPort(\"";
	    galSetPorts += ph->sanitizedName();
	    galSetPorts += "\", this, ";
	    galSetPorts += ph->myType();
	    galSetPorts += ");\n";
	    // generate the alias
	    // We should go down one level of aliasing only
	    // The cast is OK because the alias of a PortHole is always PortHole
	    const GenericPort* pha = ph->alias();
	    portAliases += indent(1);
	    portAliases += "alias(";
	    portAliases += ph->sanitizedName();
	    portAliases += ", ";
	    portAliases += ph->alias()->parent()->sanitizedName();
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
	runCode += galState->sanitizedName();
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
	runCode += galState->sanitizedName();
	runCode += ".setState(\"";
	runCode += galState->sanitizedName();
	runCode += "\", this, \"";
	runCode += galState->getInitValue();
	runCode += "\"));\n";
    }
    next.reset();
    while ((b = next++) != 0) {
	// Add the stars and galaxies used to the knownblock list
	runCode += indent(1);
	runCode += "addBlock(";
	runCode += b->sanitizedName();
	runCode += ",\"";
	runCode += b->sanitizedName();
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
	    runCode += b->sanitizedName();
	    runCode += ".setState(\"";
	    runCode += s->sanitizedName();
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
	const GenericPort *g, *gt;
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
		    runCode += b->sanitizedName();
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
		    runCode += gp->parent()->sanitizedName();
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
    

static CompileTarget compileTargetProto;
static KnownTarget entry(compileTargetProto,"compile-SDF");
