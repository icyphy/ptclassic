static const char file_id[] = "CGCTarget.cc";
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

 Programmer: S. Ha, E. A. Lee

 Base target for C code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCTarget.h"
#include "CGDisplay.h"
#include "CGUtilities.h"
#include "CGCStar.h"
#include "GalIter.h"
#include "miscFuncs.h"
#include "dataType.h"
#include "EventHorizon.h"
#include "SDFScheduler.h"

extern const char* whichType(DataType);

// constructor
CGCTarget::CGCTarget(const char* name,const char* starclass,
                   const char* desc) : HLLTarget(name,starclass,desc) {
	addState(staticBuffering.setState("staticBuffering",this,"YES",
                        "static buffering is enforced between portholes."));
	addState(hostMachine.setState("hostMachine",this,"",
                        "Machine on which to compile/run the generated code"));
	addState(funcName.setState("funcName",this,"main",
                        "function name to be created."));
        addState(doCompile.setState("doCompile",this,"YES",
                "disallow compiling during development stage"));
	addState(compileCommand.setState("compileCommand",this,"cc",
                        "function name to be created."));
	addState(compileOptions.setState("compileOptions",this,"",
                        "options to be specified for compiler."));
	addState(linkOptions.setState("linkOptions",this,"-lm",
                        "options to be specified for linking."));
	addState(saveFileName.setState("saveFileName",this,"",
                        "file name to save the generated code."));
	initCodeStrings();

	// stream definition
	addStream("globalDecls", &globalDecls);
	addStream("galStruct", &galStruct);
	addStream("include", &include);
	addStream("mainDecls", &mainDecls);
	addStream("mainInit", &mainInit);
	addStream("commInit", &commInit);
	addStream("mainClose", &mainClose);
}

StringList CGCTarget :: sectionComment(const char* s) {
	StringList out = "\n/****** ";
	out << s << " ******/\n";
	return out;
}

void CGCTarget :: headerCode () {
	StringList code = "Generated C code for target ";
	code += fullName();
	StringList tmp = sectionComment(code);
	globalDecls << tmp;
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
	if (p->switched()) continue;
	out += block->declarePortHole(p); // declare porthole
	out += block->declareOffset(p);   // declare offset
    }

    // Continue with the referenced state variables
    StateListIter nextState(block->referencedStates);
    const State* s;
    while ((s = nextState++) != 0) {
	out += block->declareState(s); // declare and initialize state
    }

    if (block->emptyFlag == 0) (*getStream("galStruct")) += out;
}

void CGCTarget :: setup() {
	// Note that code strings are not initialized here since
	// parent target may want to put some code before calling
	// setup() method.
	// Instead, initCodeStrings() is called in frameCode() method
	// after contructing the whole code.

	// member initialize
	galId = 0; curId = 0;

	if (galaxy()) setStarIndices(*galaxy()); 
	CGTarget::setup();
}

void CGCTarget :: initCodeStrings() {
	globalDecls.initialize();
	galStruct.initialize();
	procedures.initialize();
	include.initialize();
	mainDecls.initialize();
	mainInit.initialize();
	commInit.initialize();
	wormIn.initialize();
	wormOut.initialize();
	mainClose.initialize();
}
	
static char* complexDecl = 
"\n#if !defined(COMPLEX_DATA)\n#define COMPLEX_DATA 1"
"\n typedef struct complex_data { double real; double imag; } complex; \n"
"#endif\n";

void CGCTarget :: frameCode () {
    Galaxy* gal = galaxy();
    // Data structure declaration
    StringList tmp = sectionComment("Data structure declations");
    galStruct += tmp;
    galDataStruct(*gal,1);

    // Assemble all the code segments
    StringList runCode = include;
    runCode += complexDecl;
    runCode += globalDecls;
    runCode += galStruct;
    runCode += procedures;
    tmp = sectionComment("Main function");
    runCode += tmp;
    runCode += (const char*) funcName;
    runCode += "() {\n";
    runCode += mainDecls;
    runCode += commInit;
    runCode += mainInit;
    runCode += myCode;
    runCode += mainClose;
    runCode += "}\n";
    
    myCode.initialize();
    myCode += runCode;

    // after generating code, initialize code strings again.
    initCodeStrings();
}

void CGCTarget :: writeCode(const char* name) {
	if (name == NULL) {
		StringList fname;
		fname << galaxy()->name() << ".c"; 
		CGTarget :: writeCode(fname);
	} else {
		CGTarget :: writeCode(name);
	}
}

void CGCTarget :: wrapup () {
	display(myCode);
	if (galaxy() && inWormHole() == FALSE) wormLoadCode();
}

int CGCTarget :: wormLoadCode() {
	if (int(doCompile) == 0) return TRUE;
	if(compileCode()) runCode();
	if(Scheduler::haltRequested()) return FALSE;
	return TRUE;
}
	
// check whether the hostMachine is the same as my hostname.
void CGCTarget :: checkHostMachine() {
	localHost = FALSE;
	FILE* fp = popen("/bin/hostname", "r");
	if (fp == NULL) {
		Error::warn("popen error");
	} else {
		char line[40];
		if (fgets(line, 40, fp) != NULL) {
			char* myHost = makeLower(line);
			char* temp = makeLower((const char*) hostMachine);
			if (strncmp(myHost, temp, strlen(temp)) == 0) {
				localHost = TRUE;
		   	}
			LOG_DEL; delete temp;
			LOG_DEL; delete myHost;
		}
	}
	pclose(fp);    
}

// compile the code
int CGCTarget :: compileCode() {
	// check whether the hostMachine is the same as my hostname.
	checkHostMachine();

	// Compile and run the code
	StringList fname;
	fname << galaxy()->name() << ".c";
	char* tempName = writeFileName(fname);
	const char* dirName = (const char*) destDirectory;
	StringList tempCmd = compileLine(fname);

	StringList cmd;
	if (localHost == FALSE) {
		// move the file first. Create destDirectory if necessary.
		cmd << "/bin/cat " << tempName;
		cmd << " | rsh " << (const char*) hostMachine << " '";
		cmd << "mkdir -p " << dirName << "; cd " << dirName;
		cmd << "; /bin/cat - > " << fname << ".tmp ";
		cmd << "; mv -f " << fname << ".tmp " << fname << "; ";
	} else {
		cmd << "cd " << (const char*)destDirectory << "; ";
	}
	// compile the file.
	cmd << tempCmd << " -o " << galaxy()->name();
	if (localHost == FALSE) cmd << "'";
	if(system(cmd)) {
		StringList err = " Can not compile ";
		err << fname;
		if (localHost == FALSE) {
			err << " in machine: " << (const char*) hostMachine;
		}
		Error::abortRun(err);
		return FALSE;
	}
	LOG_DEL; delete tempName;
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
	StringList fname = galaxy()->name();
	StringList cmd = "cd ";
	cmd << (const char*)destDirectory;
	cmd << "; " << fname << " &";
	if (localHost == FALSE) {
		// use rshSystem method to preserve x environ.
		if (rshSystem((const char*) hostMachine, cmd, 0)) {
			StringList err = " Can not run ";
			err << fname << " in machine: ";
			err << (const char*) hostMachine;
			Error :: abortRun(err);
			return FALSE;
		}
	} else {
		if(system(cmd)) {
			StringList err = " Can not run ";
			err << fname;
			Error :: abortRun(err);
			return FALSE;
		}
	}

	// Move the code into files if saveFileName is given.
	const char* ch = (const char*) saveFileName;
	if (ch == 0 || *ch == 0) return TRUE;
	cmd = "cd ";
	cmd << (const char*)destDirectory;
	cmd << "; mv -f " << fname << ".c " << ch << ".c; mv ";
	cmd << fname << " " << ch;
	system(cmd);
	return TRUE;
}

// Routines for writing code: schedulers may call these
void CGCTarget::beginIteration(int repetitions, int depth) {
	myCode << indent(depth);
        if (repetitions == -1)          // iterate infinitely
                myCode += "while(1) {\n";
        else {
	    mainDecls << "    "
		      << "int " << targetNestedSymbol.push("i") << ";\n";
	    myCode << "for (" << targetNestedSymbol.get() << "=0; "
		   << targetNestedSymbol.get() << " < " << repetitions << "; "
	    	   << targetNestedSymbol.pop() << "++) {\n";
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
	    if (p->isItOutput() && (!p->switched())) {
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
// this routine just determines the buffer sizes, and buffer properties.

int CGCTarget :: allocateMemory() {
	int loop = int(loopingLevel);
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
			// for loop scheduler, giveup linear buffering if
			// rate change occurs.
			if (loop) {
				if (p->far() && (!p->atBoundary())) {
					int nMe = p->numXfer();
					CGCPortHole* farP = 
						(CGCPortHole*) p->far();
					int nFar = farP->numXfer();
					if ((nMe < nFar) || (nMe%nFar != 0) ||
					    (p->numInitDelays() > 0) || 
					    p->usesOldValues() ||
					    farP->usesOldValues() )
						p->giveUpStatic();
				}
			}
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
	switchCodeStream(galaxy(), getStream("mainInit"));
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
				StringList temp = appendedName(*p, "copy_ix");
				out << "    ";
				out << temp;
				out << " = (" << f->numXfer() << " + ";
				out << temp;
				out << ") % " << p->bufSize() << ";\n";
			}
		}
	}
	return out;
}
			
/////////////////////////////////////////
// incrementalAdd
/////////////////////////////////////////

int CGCTarget :: incrementalAdd(CGStar* s, int flag) {

	CGCStar* cs = (CGCStar*) s;
	if (!flag) {
		// run the star
		switchCodeStream(cs, getStream("code"));
		writeFiring(*cs, 1);
		return TRUE;
	}

	// initialize the star
	s->initialize();

	// allocate resource
	BlockPortIter nextPort(*s);
	CGCPortHole* p;
	while ((p = (CGCPortHole*)nextPort++) != 0) {

		p->finalBufSize(useStaticBuffering());
	  	p->initOffset(); 

		// Assign names only for each output port
		if (p->isItOutput() && (p->switched() == 0)) {
			StringList sl = sanitizedFullName(*p);
			p->setGeoName(savestring(sl));
	    }
	}

	// initCode
	switchCodeStream(cs, getStream("mainInit"));
	cs->initBufPointer();
	cs->initCode();

	// run the star
	switchCodeStream(cs, getStream("code"));
	writeFiring(*cs, 1);

	switchCodeStream(cs, getStream("mainClose"));
	cs->wrapup();

	// data structure for the star
	starDataStruct(cs);

	switchCodeStream(cs, getStream("code"));
	return TRUE;
}
	
// maintain the galaxy id properly.
int CGCTarget :: insertGalaxyCode(Galaxy* g, SDFScheduler* s) {
	int saveId = galId;
	curId++;
	galId = curId;
	setStarIndices(*g);
	if (!CGTarget :: insertGalaxyCode(g, s)) return FALSE;
	galDataStruct(*g, 1);
	galId = saveId;
	return TRUE;
}

// redefine compileRun to switch code stream of stars
void CGCTarget :: compileRun(SDFScheduler* s) {
	switchCodeStream(galaxy(), getStream("code"));
	s->compileRun();
	switchCodeStream(galaxy(), getStream("mainClose"));
}

/////////////////////////////////////////
// Utilities
/////////////////////////////////////////

StringList CGCTarget :: sanitizedFullName (const NamedObj& obj) const {
	StringList out;
	out << 'g' << galId << '_';
	Star* s = (Star*) obj.parent();
	out << sanitizedName(*s) << '_';
	if (s->index() >= 0) out << s->index() << '_';
	out << sanitizedName(obj);
	return out;
}

StringList CGCTarget :: appendedName (const NamedObj& p, const char* s) {
	StringList out = sanitizedFullName(p);
	out += '_';
	out += s;
	return out;
}

StringList CGCTarget :: offsetName(const CGCPortHole* p) {
	StringList temp = appendedName(*p, "ix");
	return temp;
}

const char* whichType(DataType s) {
	if ((strcmp(s, INT) == 0) || (strcmp(s, "INTARRAY") == 0)) 
		return "int";
	else if ((strcmp(s, COMPLEX) == 0) || (strcmp(s, "COMPLEXARRAY") == 0)) 
		return "complex";
	else if (strcmp(s, "STRING") == 0) return "char*";
	return "double";
}
	
