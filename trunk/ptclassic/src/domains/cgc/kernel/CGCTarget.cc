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
#include "EventHorizon.h"

extern const char* whichType(DataType);

// constructor
CGCTarget::CGCTarget(const char* name,const char* starclass,
                   const char* desc) : HLLTarget(name,starclass,desc) {
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
	initCodeStrings();

	// stream definition
	addStream("globalDecls", &globalDecls);
	addStream("include", &include);
	addStream("mainDecls", &mainDecls);
	addStream("mainInit", &mainInit);
}

StringList CGCTarget :: sectionComment(StringList s) {
	StringList out = comment(s, " /**** ", " *****/");
	return out;
}

void CGCTarget :: headerCode () {
	globalDecls << headerComment("/* ", " */", " * ");
}


// galaxy declaration
void CGCTarget :: galDataStruct(Galaxy& galaxy, int) {
    GalStarIter next(galaxy);
    CGCStar* b;
    while ((b = (CGCStar*) next++) != 0) {
	if (!b->isItFork()) starDataStruct(b);
    }
}

void CGCTarget :: starDataStruct(CGCStar* block, int) {
    
    StringList tmp = "Star: ";
    tmp += block->fullName();

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
	out += block->declareState(s);	// declare and initialize state
    }

    if (block->emptyFlag == 0) globalDecls += out;
}

void CGCTarget :: setup() {
    // member initialize
    galId = 0;
    unique = 0;

    if (galaxy()) setStarIndices(*galaxy()); 
    CGTarget::setup();
}

void CGCTarget :: initCodeStrings() {
	globalDecls.initialize();
	procedures.initialize();
	include.initialize();
	mainDecls.initialize();
	mainInit.initialize();
	wormIn.initialize();
	wormOut.initialize();
}
	
static char* complexDecl = 
"\n typedef struct complex_data { double real; double imag; } complex; \n";

void CGCTarget :: frameCode () {
    Galaxy* gal = galaxy();
    // Data structure declaration
    globalDecls += sectionComment("Data structure declations");
    galDataStruct(*gal,1);

    // Assemble all the code segments
    StringList runCode = include;
    runCode += complexDecl;
    runCode += globalDecls;
    runCode += procedures;
    runCode += sectionComment("Main function");
    runCode += (const char*) funcName;
    runCode += "() {\n";
    runCode += mainDecls;
    runCode += mainInit;
    runCode += myCode;
    
    myCode.initialize();
    myCode += runCode;

    // after generating code, initialize code strings again.
    initCodeStrings();
}

void CGCTarget :: generateCode() {
	CGTarget :: generateCode();
	myCode += "}\n";
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
	cmd += compileLine("code.c");
	if(system(cmd)) {
		Error::abortRun("Compilation errors in generated code.");
		return FALSE;
	}
	return TRUE;
}

// return compile command
StringList CGCTarget :: compileLine(const char* fName) {
	StringList cmd = (const char*) compileCommand;
	cmd << " " << (const char*) compileOptions << " ";
	cmd << fName << " " << (const char*) linkOptions;
	return cmd;
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
	if (ch == 0 || *ch == 0) ch = galaxy()->name();
	cmd << ch << ".c; mv a.out " << ch;
	system(cmd);
	return TRUE;
}

// Routines for writing code: schedulers may call these
void CGCTarget::beginIteration(int repetitions, int depth) {
	myCode << indent(depth);
        if (repetitions == -1)          // iterate infinitely
                myCode += "while(1) {\n";
        else {
                mainDecls += indent(1);
                mainDecls += "int i";
                mainDecls += unique;
                mainDecls += ";\n";
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
	myCode += wormIn;
        return;
}

void CGCTarget :: wormInputCode(PortHole& p) {
	wormIn << "\t/* READ from wormhole port " 
		<< p.fullName() << " */\n";
}

void CGCTarget :: wormOutputCode(PortHole& p) {
	wormOut << "\t/* WRITE to wormhole port " 
		<< p.fullName() << " */\n";
}

void CGCTarget :: endIteration(int /*reps*/, int depth) {
	myCode << wormOut;
	myCode << updateCopyOffset();
	myCode << "} /* end repeat, depth " << depth << "*/\n";
}

// clone
Block* CGCTarget :: makeNew () const {
	LOG_NEW; return new CGCTarget(name(),starType(),descriptor());
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

int CGCTarget :: allocateMemory() {
	Galaxy& g = *galaxy();
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

int CGCTarget :: codeGenInit() {

        // initialize the porthole offsets, and do all initCode methods.
        GalStarIter nextStar(*galaxy());
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
	setGeoNames(*galaxy());

	// call initialization code.
	nextStar.reset();
	while ((s = (CGCStar*) nextStar++) != 0) {
		if (s->isItFork()) continue;
		s->initBufPointer();
		s->initCode();
	}

        return TRUE;
}

/////////////////////////////////////////
// updateCopyOffset
/////////////////////////////////////////

StringList CGCTarget :: updateCopyOffset() {
	StringList out;
	
	GalStarIter nextStar(*galaxy());
	CGCStar* s;
	while ((s = (CGCStar*) nextStar++) != 0) {
		if (s->isItFork()) continue;
		BlockPortIter next(*s);
		CGCPortHole* p;
		while ((p = (CGCPortHole*) next++) != 0) {
			if (p->isItOutput()) continue;
			CGCPortHole* f = (CGCPortHole*) p->far();
			if ((f->embedded() && f->bufType() == OWNER) ||
			    (f->embedding() && f->bufType() == COPIED)) {
				out << "    ";
				out << appendedName(*p, "copy_ix");
				out << " = (" << f->numXfer() << " + ";
				out << appendedName(*p, "copy_ix");
				out << ") % " << p->bufSize() << ";\n";
			}
		}
	}
	return out;
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

StringList CGCTarget :: appendedName (const NamedObj& p, const char* s) {
	StringList out = sanitizedFullName(p);
	out += '_';
	out += s;
	return out;
}

StringList CGCTarget :: offsetName(const CGCPortHole* p) {
	return appendedName(*p, "ix");
}

const char* whichType(DataType s) {
	if ((strcmp(s, INT) == 0) || (strcmp(s, "INTARRAY") == 0)) 
		return "int";
	else if ((strcmp(s, COMPLEX) == 0) || (strcmp(s, "COMPLEXARRAY") == 0)) 
		return "complex";
	else if (strcmp(s, "STRING") == 0) return "char*";
	return "double";
}
	
