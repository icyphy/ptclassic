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
#include "CGDisplay.h"
#include "CGCStar.h"
#include "GalIter.h"
#include "miscFuncs.h"
#include "dataType.h"
#include "WormConnect.h"

extern const char* whichType(DataType);

// constructor
CGCTarget::CGCTarget(const char* name,const char* starclass,
                   const char* desc) : BaseCTarget(name,starclass,desc) {
	addState(staticBuffering.setState("staticBuffering",this,"YES",
                        "static buffering is enforced between portholes."));
	addState(funcName.setState("funcName",this,"main",
                        "function name to be created."));
	addState(compileCommand.setState("compileCommand",this,"cc",
                        "function name to be created."));
	addState(compileOptions.setState("compileOptions",this,"",
                        "options to be specified for compiler."));
	addState(linkOptions.setState("linkOptions",this,"",
                        "options to be specified for linking."));
	addState(saveFileName.setState("saveFileName",this,"",
                        "file name to save the generated code."));
}

StringList CGCTarget :: sectionComment(const StringList s) {
	StringList out = "\n/****** ";
	out += s;
	out += " ******/\n";
	return out;
}

StringList CGCTarget::offsetName(const CGCPortHole* p) {
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


int CGCTarget :: galDataStruct(Galaxy& galaxy, int) {
    GalStarIter next(galaxy);
    CGCStar* b;
    while ((b = (CGCStar*) next++) != 0) {
	if (!b->isItFork()) starDataStruct(b);
    }
    return TRUE;
}

int CGCTarget :: starDataStruct(CGCStar* block, int) {
    
    StringList tmp = "Star: ";
    tmp += block->readFullName();

    StringList out = sectionComment(tmp);

    // Start with the PortHoles
    BlockPortIter nextPort(*block);
    CGCPortHole* p;
    while ((p = (CGCPortHole*) nextPort++) != 0) {
	out += block->declarePortHole(p);	// declare porthole
	out += block->declareOffset(p);	// declare offset
    }

    // Continue with the referenced state variables
    StateListIter nextState(block->referencedStates);
    const State* s;
    while ((s = nextState++) != 0) {
	out += block->declareState(s);	// declare state

	// Initialize the state
	mainInitialization += block->initializeState(s);	
    }

    if (block->emptyFlag == 0) staticDeclarations += out;
    return(TRUE);
}

void CGCTarget :: start() {
	galId = 0;
	CGTarget :: start();
}

int CGCTarget :: setup (Galaxy& g) {

    // Initializations
    include = "";
    mainDeclarations = "";
    mainInitialization = "";
    procedures = "";
    unique = 0;

    includeFiles.initialize();
    globalDecls.initialize();

    setStarIndices(g); 

    return CGTarget::setup(g);
}

void CGCTarget :: frameCode () {

    // Data structure declaration
    StringList leader = "Code from Universe: ";
    leader += gal->readFullName();
    staticDeclarations += sectionComment(leader);
    staticDeclarations += sectionComment("Data structure declations");
    galDataStruct(*gal,1);

    // Assemble all the code segments
    StringList runCode = include;
    runCode += staticDeclarations;
    runCode += procedures;
    runCode += sectionComment("Main function");
    runCode += (const char*) funcName;
    runCode += "() {\n";
    runCode += mainDeclarations;
    runCode += mainInitialization;
    runCode += myCode;
    runCode += indent(1);
    
    myCode = runCode;
}

StringList CGCTarget :: generateCode(Galaxy& g) {
	setup(g);
	run();
	myCode += "}\n";
	return myCode;
}

void CGCTarget :: wrapup () {

	if(Scheduler::haltRequested()) return;
	Target::wrapup();

	myCode += indent(1);
	myCode += "exit(0);\n";
	myCode += "}\n";

	// display code
	char* codeFileName = writeFileName("code.c");
	if(!display(myCode, codeFileName)) return;

	if(compileCode()) runCode();
}
	
// compile the code
int CGCTarget :: compileCode() {

	// Compile and run the code
	StringList cmd = "cd ";
	cmd += (const char*)destDirectory;
	cmd += "; ";
	cmd += (const char*)compileCommand;
	cmd += " ";
	cmd += (const char*)compileOptions;
	cmd += " code.c ";
	cmd += (const char*)linkOptions;
	if(system(cmd)) {
		Error::abortRun("Compilation errors in generated code.");
		return FALSE;
	}
	return TRUE;
}

// down-load (do nothing here) and run the code
int CGCTarget :: runCode() {
	StringList cmd = "cd ";
	cmd += (const char*)destDirectory;
	cmd += "; a.out";
	if(system(cmd)) {
		Error::abortRun("Error(s) executing the generated program.");
		return FALSE;
	}
	// Move the code into files of more reasonable names
	cmd = "cd ";
	cmd += (const char*)destDirectory;
	cmd += "; mv -f code.c ";
	const char* ch = (const char*) saveFileName;
	if (ch && strcmp(ch, "")) {
		cmd += (const char*) saveFileName;
		cmd += ".c; mv a.out ";
		cmd += (const char*) saveFileName;
	} else {
		cmd += gal->readName();
		cmd += ".c; mv a.out ";
		cmd += gal->readName();
	}
	system(cmd);
	return TRUE;
}

// Routines for writing code: schedulers may call these
void CGCTarget::beginIteration(int repetitions, int depth) {
	myCode << indent(depth);
        if (repetitions == -1)          // iterate infinitely
                myCode += "while(1) {\n";
        else {
                mainDeclarations += indent(1);
                mainDeclarations += "int i";
                mainDeclarations += unique;
                mainDeclarations += ";\n";
                myCode += "for (i";
                myCode += unique;
                myCode += "=0; i";
                myCode += unique;
                myCode += " < ";
                myCode += repetitions;
                myCode += "; i";
                myCode += unique++;
                myCode += "++) {\n";
        }
        return;
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

void CGCTarget :: addProcedure(const char* decl) {
	procedures += decl;
}

void CGCTarget :: addMainInit(const char* decl) {
	mainInitialization += indent(1);
	mainInitialization += decl;
}

// clone
Block* CGCTarget :: clone () const {
	LOG_NEW; 
	CGCTarget* t = new CGCTarget(readName(),starType(),readDescriptor());
	t->copyStates(*this);
	return t;
}

void CGCTarget :: setGeoNames(Galaxy& galaxy) {
    GalStarIter next(galaxy);
    Star* b;
    while ((b = next++) != 0) {
	BlockPortIter nextPort(*b);
	CGCPortHole* p;
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

// note that we allow the C compiler to do the actual allocation;
// this routine just determines the buffer sizes.

int CGCTarget :: allocateMemory(Galaxy& g) {
	// set up the forkDests members of each Fork inputs.
	setupForkDests(g);
	
	GalStarIter nextStar(g);
	CGCStar* s;
	while ((s = (CGCStar*)nextStar++) != 0) {
		if (s->isItFork()) continue;
		BlockPortIter next(*s);
		CGCPortHole* p;
		while ((p = (CGCPortHole*) next++) != 0) {
			p->finalBufSize(useStaticBuffering());
		}
	}

	return TRUE;
}

void CGCTarget :: setupForkDests(Galaxy& g) {
	GalStarIter nextStar(g);
	CGCStar* s;
	while ((s = (CGCStar*)nextStar++) != 0) {
		if (!s->isItFork()) continue;
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
		if (s->isItFork()) continue;
		s->initBufPointer();
		s->initCode();
	}

        return TRUE;
}

StringList CGCTarget :: sanitizedFullName (const NamedObj& obj) const {
	StringList out;
	out << 'g' << galId << '_';
	Star* s = (Star*) obj.parent();
	out += sanitizedName(*s);
	out << '_' << s->index() << '_';
	out += sanitizedName(obj);
	return out;
}

const char* whichType(DataType s) {
	if ((strcmp(s, INT) == 0) || (strcmp(s, "INTARRAY") == 0)) 
		return "int";
	else if (strcmp(s, "STRING") == 0) return "char*";
	return "double";
}
	
