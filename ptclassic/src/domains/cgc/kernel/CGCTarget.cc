static const char file_id[] = "CGCTarget.cc";
/******************************************************************
Version identification:
$Id$

 Copyright (c) 1992 The Regents of the University of California.
                       All Rights Reserved.

 Programmer: E. A. Lee

 Base target for C code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCTarget.h"
#include "FloatArrayState.h"
#include "CGCStar.h"
#include "GalIter.h"
#include "miscFuncs.h"

const Attribute ANY(0,0);

// constructor
CGCTarget::CGCTarget() : BaseCTarget("default-CGC","CGCStar",
"Generate stand-alone C programs and compile them.  The program\n"
"and associated makefile is written to a directory given as a Target param.\n"
"Can use either the default SDF scheduler or Joe's loop scheduler.")
{ }

StringList CGCTarget :: sectionComment(const StringList s) {
	StringList out = "\n/****** ";
	out += s;
	out += " ******/\n\n";
	return out;
}

StringList CGCTarget::offsetName(CGCPortHole* p) {
        StringList out = sanitizedFullName(*p);
        out += "_ix";
        return out;
}

void CGCTarget :: headerCode () {
        StringList code = "Generated C code for target ";
        code += readFullName();
	StringList tmp = sectionComment(code);
	staticDeclarations = tmp;
}


int CGCTarget :: galDataStruct(Galaxy& galaxy, int level) {
    GalTopBlockIter next(galaxy);
    Block* b;
    while ((b = next++) != 0) {
	if(!b->isItAtomic()) {
	    staticDeclarations += indent(level);
	    staticDeclarations += "struct {\n";
	    galDataStruct(b->asGalaxy(), level+1);
	    staticDeclarations += indent(level);
	    staticDeclarations += "} ";
	    staticDeclarations += sanitize(b->readName());
	    staticDeclarations += ";\n";
	} else {
	    CGCStar* s = (CGCStar*) b;
	    if (!s->amIFork()) starDataStruct(*b, level);
	}
    }
    return TRUE;
}

int CGCTarget :: starDataStruct(Block& block, int level) {
    int emptyFlag = TRUE;
    StringList out = indent(level);
    out += "struct {\n";

    // Start with the PortHoles
    BlockPortIter nextPort(block);
    CGCPortHole* p;
    while ((p = (CGCPortHole*) nextPort++) != 0) {
	// Define variables only for each output port, except outputs of forks
	if (p->isItOutput()) {
	   	int dimen = ((CGCPortHole*)p)->maxBufReq();
		out += indent(level+1);
		out += "float ";
		out += sanitizedName(*p);
		if(dimen > 1) {
		    out += "[";
		    out += dimen;
		    out += "]";
		}
		out += ";\n";
		emptyFlag = FALSE;
	}
	// store offsets too.
	if (p->maxBufReq() > 1) {
		emptyFlag = FALSE;
		out += indent(level+1);
		out += "int ";
		out += sanitizedName(*p);
		out += "_ix";		// suffix to indicate the offset.
		out += ";\n";
	}
    }

    // Continue with the referenced state variables
    StateListIter nextState(((CGCStar&)block).referencedStates);
    const State* s;
    while ((s = nextState++) != 0) {
	out += indent(level+1);
	out += "float ";
	out += sanitizedName(*s);
	if (s->size() > 1) {
		out += "[";
		out += s->size();
		out += "]";
	}
	out += ";\n";
	emptyFlag = FALSE;

	// Initialize the state
	mainInitialization += indent(1);
	if (s->size() > 1) {
		FloatArrayState* fs = (FloatArrayState*) s;
		for (int i = 0; i < s->size(); i++) {
			mainInitialization += "    ";
			mainInitialization += sanitizedFullName(*s);
			mainInitialization += "[";
			mainInitialization += i;
			mainInitialization += "] = ";
			mainInitialization += (*fs)[i];
			mainInitialization += ";\n";
		}
	} else {
		mainInitialization += sanitizedFullName(*s);
		mainInitialization += " = ";
		mainInitialization += s->getInitValue();
		mainInitialization += ";\n";
	}
    }

    out += indent(level);
    out += "} ";
    out += sanitizedName(block);
    out += ";\n";
    if (!emptyFlag) staticDeclarations += out;
    return(TRUE);
}

int CGCTarget :: setup (Galaxy& g) {

    // Initializations
    include = "";
    mainDeclarations = "";
    mainInitialization = "";
    unique = 0;

    includeFiles.initialize();
    globalDecls.initialize();

    return CGTarget::setup(g);
}

int CGCTarget :: run () {

    // Main loop runs before data structure declarations so that
    // the states that are referenced with $ref() macros can be
    // marked as states that must be declared and initialized.
    BaseCTarget::run();

    // Data structure declaration
    StringList leader = "Code from Universe: ";
    leader += gal->readFullName();
    staticDeclarations += sectionComment(leader);
    staticDeclarations += sectionComment("Data structure declations");
    staticDeclarations += "struct {\n";
    galDataStruct(*gal,1);
    staticDeclarations += "} ";
    staticDeclarations += sanitize(gal->readName());
    staticDeclarations += ";\n";

    // Assemble all the code segments
    StringList runCode = include;
    runCode += staticDeclarations;
    runCode += sectionComment("Main function");
    runCode += "main() {\n";
    runCode += mainDeclarations;
    runCode += mainInitialization;
    runCode += myCode;
    runCode += indent(1);
    
    myCode = runCode;

    return(TRUE);
}

void CGCTarget :: wrapup () {

	if(Scheduler::haltRequested()) return;
	Target::wrapup();

	myCode += indent(1);
	myCode += "exit(0);\n";
	myCode += "}\n";

	char* codeFileName = writeFileName("code.c");
	UserOutput codeFile;
	if(!codeFile.fileName(codeFileName)) {
	    Error::abortRun("Can't open code file for writing: ",codeFileName);
	    return;
	}
	writeCode(codeFile);

	// Display the code
	Error::message(myCode);

	// Compile and run the code
	StringList cmd = "cd ";
	cmd += (const char*)destDirectory;
	cmd += "; cc code.c";
	if(system(cmd)) {
		Error::abortRun("Compilation errors in generated code.");
		return;
	}
	cmd = "cd ";
	cmd += (const char*)destDirectory;
	cmd += "; a.out";
	if(system(cmd)) {
		Error::abortRun("Error(s) executing the generated program.");
		return;
	}
	// Move the code into files of more reasonable names
	cmd = "cd ";
	cmd += (const char*)destDirectory;
	cmd += "; mv -f code.c ";
	cmd += gal->readName();
	cmd += ".c; mv a.out ";
	cmd += gal->readName();
	system(cmd);
}

// Routines for writing code: schedulers may call these
StringList CGCTarget::beginIteration(int repetitions, int depth) {
        StringList out;
        out = indent(depth);
        if (repetitions == -1)          // iterate infinitely
                out += "while(1) {\n";
        else {
                mainDeclarations += indent(1);
                mainDeclarations += "int i";
                mainDeclarations += unique;
                mainDeclarations += ";\n";
                out += "for (i";
                out += unique;
                out += "=0; i";
                out += unique;
                out += " < ";
                out += repetitions;
                out += "; i";
                out += unique++;
                out += "++) {\n";
        }
        return out;
}

void CGCTarget :: addInclude(const char* inc) {

	// check whether the file is included or not.
	StringListIter next(includeFiles);
	const char* p;

	while ((p = next++) != 0) {
		if (!strcmp(inc,p)) return;
	}

	// add new file
	includeFiles += inc;
	StringList out = "#include ";
	out += inc;
	out += "\n";
	include += out;
}

void CGCTarget :: addGlobal(const char* decl) {

	// check whether the file is included or not.
	StringListIter next(globalDecls);
	const char* p;

	while ((p = next++) != 0) {
		if (!strcmp(decl,p)) return;
	}

	// add new file
	globalDecls += decl;
	staticDeclarations += decl;
}

void CGCTarget :: addDeclaration(const char* decl) {
	mainDeclarations += indent(1);
	mainDeclarations += decl;
}

void CGCTarget :: addMainInit(const char* decl) {
	mainInitialization += indent(1);
	mainInitialization += decl;
}

// copy constructor
CGCTarget :: CGCTarget (const CGCTarget& src) :
BaseCTarget(src.readName(), "CGCStar", src.readDescriptor())
{ }

// clone
Block* CGCTarget :: clone () const {
	LOG_NEW; return new CGCTarget(*this);
}

void CGCTarget :: setGeoNames(Galaxy& galaxy) {
    GalStarIter next(galaxy);
    Star* b;
    while ((b = next++) != 0) {
	BlockPortIter nextPort(*b);
	const CGCPortHole* p;
	while ((p = (CGCPortHole*)nextPort++) != 0) {
	    // Assign names only for each output port
	    if (p->isItOutput()) {
		StringList s = sanitizedFullName(*p);
		p->setGeoName(savestring(s));
	    }
	}
    }
}

/////////////////////////////////////////
// routines to determine the buffer sizes
/////////////////////////////////////////

int CGCTarget :: decideBufSize(Galaxy& g) {
	// set up the forkDests members of each Fork inputs.
	setupForkDests(g);
	
	GalStarIter nextStar(g);
	CGCStar* s;
	while ((s = (CGCStar*)nextStar++) != 0) {
		if (s->amIFork()) continue;
		BlockPortIter next(*s);
		CGCPortHole* p;
		while ((p = (CGCPortHole*) next++) != 0) {
			p->finalBufSize();
		}
	}

	return TRUE;
}

void CGCTarget :: setupForkDests(Galaxy& g) {
	GalStarIter nextStar(g);
	CGCStar* s;
	while ((s = (CGCStar*)nextStar++) != 0) {
		if (!s->amIFork()) continue;
		BlockPortIter next(*s);
		CGCPortHole* p = (CGCPortHole*) next++;
		if (p->isItOutput()) p = p->getForkSrc();
		p->setupForkDests();
	}
}


/////////////////////////////////////////
// codeGenInit
/////////////////////////////////////////

int CGCTarget :: codeGenInit(Galaxy& g) {

        // initialize the porthole offsets, and do all initCode methods.
        GalStarIter nextStar(g);
        CGCStar* s;
        while ((s = (CGCStar*)nextStar++) != 0) {
                BlockPortIter next(*s);
                CGCPortHole* p;
                while ((p = (CGCPortHole*) next++) != 0) {
                        if (!p->initOffset()) return FALSE;
                }
        }

	// Set all geodesics to contain a symbolic name that can be
	// used as the C object representing the buffer.  That name will
	// be of the form "universe.gal1.gal2.star.output", which designates
	// the output port that actually produces the data.
	setGeoNames(*gal);

	nextStar.reset();
	while ((s = (CGCStar*) nextStar++) != 0) {
		if (s->amIFork()) continue;
		s->offsetInit();
		s->initCode();
	}

        return TRUE;
}

