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

 Programmer: S. Ha, E. A. Lee, and T. M. Parks

 Base target for C code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCTarget.h"
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
	addState(funcName.setState("funcName",this,"main",
                        "function name to be created."));
	addState(compileCommand.setState("compileCommand",this,"cc",
                        "command for compiling code."));
	addState(compileOptions.setState("compileOptions",this,"",
                        "options to be specified for compiler."));
	addState(linkOptions.setState("linkOptions",this,"-lm",
                        "options to be specified for linking."));
        addState(resources.setState("resources",this,"STDIO",
        	"standard I/O resource"));
	initCodeStrings();

	targetHost.setAttributes(A_SETTABLE);
	filePrefix.setAttributes(A_SETTABLE);
	displayFlag.setAttributes(A_SETTABLE);
	compileFlag.setAttributes(A_SETTABLE);
	runFlag.setAttributes(A_SETTABLE);

	// stream definition
	addStream("globalDecls", &globalDecls);
	addStream("include", &include);
	addStream("mainDecls", &mainDecls);
	addStream("mainInit", &mainInit);
	addStream("commInit", &commInit);
	addStream("mainClose", &mainClose);
}

StringList CGCTarget::comment(const char* text, const char* b,
    const char* e, const char* c)
{
    const char* begin = "/* ";
    const char* end = " */";
    const char* cont = "   ";
    if (b != NULL) begin = b;
    if (e != NULL) end = e;
    if (c != NULL) cont = c;
    return HLLTarget::comment(text, begin, end, cont);
}

// Galaxy declarations.
void CGCTarget::declareGalaxy(Galaxy& galaxy)
{
    GalStarIter starIter(galaxy);
    CGCStar* star;
    while ((star = (CGCStar*)starIter++) != NULL)
    {
	if (!star->isItFork()) declareStar(star);
    }
}

// Star declarations.
void CGCTarget::declareStar(CGCStar* star)
{
    // States must be declared after the Star has been run.
    // Running the Star builds the list of referenced States.

    globalDecls << star->declareStates(A_GLOBAL);
    globalDecls << star->declarePortHoles(P_GLOBAL);

    mainDecls << star->declareStates(A_LOCAL);
    mainDecls << star->declarePortHoles(P_LOCAL);

    mainInit << star->initCodeStates();
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
	HLLTarget::setup();
}

void CGCTarget :: initCodeStrings() {
	globalDecls.initialize();
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

void CGCTarget::trailerCode()
{
    declareGalaxy(*galaxy());
    if (!SimControl::haltRequested())
	HLLTarget::trailerCode();
}

// Initial stage of code generation.
void CGCTarget::headerCode()
{
    // Do nothing.
}

// Combine all sections of code.
void CGCTarget :: frameCode ()
{
    StringList code = headerComment();
    code << include << complexDecl << globalDecls << procedures
	 << comment("main function")
	 << (const char*)funcName << "() {\n"
	 << mainDecls << commInit << mainInit << myCode
	 << mainClose << "\n}\n";

    myCode.initialize();
    myCode << code;

    // after generating code, initialize code strings again.
    initCodeStrings();
}

void CGCTarget :: writeCode()
{
    writeFile(myCode,".c",displayFlag);
}

int CGCTarget::compileCode()
{
    StringList file, cmd, error;
    file << filePrefix << ".c";
    cmd << compileLine(file) << " -o " << filePrefix;
    error << "Could not compile " << file;
    return (systemCall(cmd, error, targetHost) == 0);
}

// return compile command
StringList CGCTarget :: compileLine(const char* fName) {
	StringList cmd = (const char*) compileCommand;
	cmd << " " << (const char*) compileOptions << " ";
	cmd << fName << " " << (const char*) linkOptions;
	return cmd;
}

// Run the code.
int CGCTarget :: runCode()
{
    StringList cmd, error;
    cmd << filePrefix << "&";
    error << "Could not run " << filePrefix;
    return (systemCall(cmd, error, targetHost) == 0);
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
	myCode << "} /* end repeat, depth " << depth << "*/\n";
}

// clone
Block* CGCTarget :: makeNew () const {
	LOG_NEW; return new CGCTarget(name(),starType(),descriptor());
}

/////////////////////////////////////////
// routines to determine the buffer sizes
/////////////////////////////////////////

// note that we allow the C compiler to do the actual allocation;
// this routine (1) determines the buffer sizes, and buffer properties.
// (2) splice star for copying
// (3) determine the type of buffers.
// (4) naming buffers
// (5) buffer offset initialization

int CGCTarget :: allocateMemory() {
	int loop = int(loopingLevel);
	Galaxy& g = *galaxy();
	// set up the forkDests members of each Fork inputs.
	setupForkDests(g);
	
	int statBuf = useStaticBuffering();
	if (loop) statBuf = 0;

	// (1) determine the buffer size
	GalStarIter nextStar(g);
	CGCStar* s;
	while ((s = (CGCStar*)nextStar++) != 0) {
		if (s->isItFork()) continue;
		BlockPortIter next(*s);
		CGCPortHole* p;
		while ((p = (CGCPortHole*) next++) != 0) {
			p->finalBufSize(statBuf);
		}
	}

	// splice copy stars
	addSpliceStars();

	nextStar.reset();
        while ((s = (CGCStar*)nextStar++) != 0) {
                BlockPortIter next(*s);
                CGCPortHole* p;
                while ((p = (CGCPortHole*) next++) != 0) {

			// (3) buffer type
			p->setBufferType();

			// (4) naming buffers.
	    		if (p->isItOutput() && (!p->switched())) {
				StringList name = sanitizedName(*p);
				name << separator << symbolCounter++;
				p->setGeoName(savestring(name));
	    		}

			// (5) initialize offset pointer.
                        if (!p->initOffset()) return FALSE;
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

	// call initialization code.
	switchCodeStream(galaxy(), getStream("mainInit"));
	GalStarIter nextStar(*galaxy());
	CGCStar* s;
	while ((s = (CGCStar*) nextStar++) != 0) {
		if (s->isItFork()) continue;
		// Generate PortHole initialization code before the Star is run.
		// Running the Star modifies the PortHole indices.
		mainInit << s->initCodePortHoles();
		s->initCode();

	}

        return TRUE;
}

// Splice in stars.
// Return FALSE on error.
int CGCTarget::modifyGalaxy()
{
    extern int warnIfNotConnected (Galaxy&);

    Galaxy& gal = *galaxy();
    GalStarIter starIter(gal);
    Star* star;
    const char* domain = gal.domain();

    if (warnIfNotConnected(gal)) return FALSE;

    while ((star = starIter++) != NULL)
    {
	BlockPortIter portIter(*star);
	PortHole* port;
	while ((port = portIter++) != NULL)
	{
	    // Splice in type conversion stars.
	    if (port->isItOutput()
		&& (port->type() != ANYTYPE)
		&& (port->type() != port->resolvedType()) )
	    {
		PortHole* input = port->far();	// destination input PortHole

		// Avoid re-initializing the Galaxy, which will break
		// things if this is a child in a MultiTarget. (Why? See
		// comments for CGTarget::setup() method.)  Initialize it
		// only if there is no resolved type.
		if (port->resolvedType() == NULL)
		{
		    gal.initialize();
		    if (SimControl::haltRequested()) return FALSE;
		    if (port->type() == port->resolvedType()) continue;
		}

		if (port->type() == COMPLEX)
		{
		    if (!spliceStar(input, "CxToFloat", TRUE, domain))
			return FALSE;
		}
		else if (port->resolvedType() == COMPLEX)
		{
		    if (!spliceStar(input, "FloatToCx", TRUE, domain))
			return FALSE;
		}
	    }
	}
    }
    return TRUE;
}

/////////////////////////////////////////
// addSpliceStars
/////////////////////////////////////////

static void setupBuffer(CGCStar* s, int dimen, int bufsz) {
	CGCPortHole* p = (CGCPortHole*) s->portWithName("output");
	p->setSDFParams(dimen);
	if (bufsz) {
		p->giveUpStatic();
		p->requestBufSize(bufsz);
	}
	else       p->requestBufSize(dimen);
	p->setFlags();
	p = (CGCPortHole*) s->portWithName("input");
	p->setSDFParams(dimen);
	p->setFlags();
}

// "Copy" stars are added if an input/output PortHole is a host/embedded
// PortHole and the buffer size is greater than the number of Particles
// transferred.

// NOTE:  Because this splicing takes place after the schedule has been
// constructed, the CGCStar::addSpliceStar() method is used as a hack to
// ensure that code will be generated for the new spliced stars.

void CGCTarget :: addSpliceStars() {
	const char* dom = galaxy()->domain();

	GalStarIter nextStar(*galaxy());
	CGCStar* s;
	CGCPortHole* farP;
	while ((s = (CGCStar*) nextStar++) != 0) {
                BlockPortIter next(*s);
                CGCPortHole* p;
                while ((p = (CGCPortHole*) next++) != 0) {
			CGCStar* news = 0;

			if (p->isItInput()) {
				farP = (CGCPortHole*) p->far();

				if ((p->embedded() || p->embedding()) &&
					 (p->numXfer() < p->bufSize())) {
					news = (CGCStar*)
						spliceStar(p, "Copy", 1, dom);
					CGCPortHole* tp = (CGCPortHole*) 
						news->portWithName("input");
					tp->giveUpStatic();
				}
				if (news) {
					news->setTarget(this);
					setupBuffer(news, p->numXfer(), 0);
					s->addSpliceStar(news, 0);
				}

			// output
			} else if ((p->embedded() || p->embedding()) &&
				 (p->numXfer() < p->bufSize())) {

				// special treatment for Collect star
				// since it is not a normal SDF star
				CGCStar* ps = (CGCStar*) p->parent();
				if (ps->amISpreadCollect()) {
					farP = (CGCPortHole*) p->far();
					news = (CGCStar*) spliceStar(farP,
						"Copy", 0, dom);
					setupBuffer(news, farP->numXfer(), 
						p->bufSize());
					ps = (CGCStar*) farP->parent();
					ps->addSpliceStar(news, 0);

					// set offset manually
					CGCPortHole* tp = (CGCPortHole*) 
						news->portWithName("output");
					int offset = p->numXfer() -
						farP->numInitDelays() - 1;
					if (offset < 0) offset += p->bufSize();
					tp->setOffset(offset);
					tp = (CGCPortHole*) news->
						portWithName("input");
					tp->setOffset(offset);
				} else {
					news = (CGCStar*) 
						spliceStar(p,"Copy",0,dom);	
					setupBuffer(news, p->numXfer(), 
						p->bufSize());
					s->addSpliceStar(news, 1);
				}
				p->requestBufSize(p->numXfer());
				news->setTarget(this);
			}
		}
	}
}

/////////////////////////////////////////
// incrementalAdd
/////////////////////////////////////////

int CGCTarget :: incrementalAdd(CGStar* s, int flag) {

	CGCStar* cs = (CGCStar*) s;
	cs->setTarget(this);

	if (!flag) {
		// run the star
		switchCodeStream(cs, getStream(CODE));
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
			StringList name = sanitizedName(*p);
			name << separator << symbolCounter++;
			p->setGeoName(savestring(name));
	    }
	}

	// Generate PortHole initialization code before the Star is run.
	// Running the Star modifies the PortHole indices.
	mainInit << cs->initCodePortHoles();

	switchCodeStream(cs, getStream("mainInit"));
	cs->initCode();


	// run the star
	switchCodeStream(cs, getStream(CODE));
	writeFiring(*cs, 1);

	switchCodeStream(cs, getStream("mainClose"));
	cs->wrapup();
	switchCodeStream(cs, getStream(CODE));

	declareStar(cs);
	return TRUE;
}
	
// maintain the galaxy id properly.
int CGCTarget :: insertGalaxyCode(Galaxy* g, SDFScheduler* s) {
	int saveId = galId;
	curId++;
	galId = curId;
	setStarIndices(*g);
	if (!HLLTarget :: insertGalaxyCode(g, s)) return FALSE;
	declareGalaxy(*g);
	galId = saveId;
	return TRUE;
}

// redefine compileRun to switch code stream of stars
void CGCTarget :: compileRun(SDFScheduler* s) {
	switchCodeStream(galaxy(), getStream(CODE));
	s->compileRun();
	switchCodeStream(galaxy(), getStream("mainClose"));
}

/////////////////////////////////////////
// Utilities
/////////////////////////////////////////

const char* whichType(DataType s) {
	if ((strcmp(s, INT) == 0) || (strcmp(s, "INTARRAY") == 0)) 
		return "int";
	else if ((strcmp(s, COMPLEX) == 0) || (strcmp(s, "COMPLEXARRAY") == 0)) 
		return "complex";
	else if (strcmp(s, "STRING") == 0) return "char*";
	return "double";
}
