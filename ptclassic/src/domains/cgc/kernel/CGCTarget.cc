static const char file_id[] = "CGCTarget.cc";
/**********************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: S. Ha, E. A. Lee, T. M. Parks and J. L. Pino

 Base target for C code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <unistd.h>		// Pick up R_OK for SunOS4.1 cfront
#include "CGCTarget.h"
#include "CGUtilities.h"
#include "CGCStar.h"
#include "GalIter.h"
#include "miscFuncs.h"
#include "dataType.h"
#include "EventHorizon.h"
#include "SDFScheduler.h"
#include "compat.h"		// Pickup symlink()
#include "InfString.h"		// Pick up InfString for use in pragma()

/* this modification is necessary to make a galaxy code as a function */
#define MAINDECLS (*getStream("mainDecls"))
#define MAININIT (*getStream("mainInit"))
// The symbol CODE is declared in CGTarget.h.
#define MYCODE (*getStream(CODE))
#define MAINCLOSE (*getStream("mainClose"))

// Add code to the beginning of a CodeStream instead of the end.
void prepend(StringList code, CodeStream& stream)
{
    StringList temp;
    temp << code;
    temp << stream;
    stream.initialize();
    stream << temp;
}

static TypeConversionTable cgcCnvTable[7] = {
  {  COMPLEX, 	FIX, 		"CxToFix"	},
  {  COMPLEX, 	ANYTYPE,	"CxToFloat"	},
  {  FIX,	COMPLEX,	"FixToCx"	},
  {  FIX,	FIX,		"FixToFix"	},
  {  FIX,	ANYTYPE,	"FixToFloat"	},
  {  ANYTYPE, 	COMPLEX,	"FloatToCx"	},
  {  ANYTYPE, 	FIX,		"FloatToFix"	}
};

// constructor
CGCTarget::CGCTarget(const char* name,const char* starclass,
                   const char* desc) : HLLTarget(name,starclass,desc) {
	addState(staticBuffering.setState("staticBuffering",this,"YES",
                        "static buffering is enforced between portholes."));
	addState(funcName.setState("funcName",this,"main",
                        "function name to be created."));
	addState(compileCommand.setState("compileCommand",this,"gcc",
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

	// Exported codeStreams.
	addStream("globalDecls", &globalDecls);
	addStream("include", &include);
	addStream("mainDecls", &mainDecls);
	addStream("mainInit", &mainInit);
	addStream("commInit", &commInit);
	addStream("compileOptions", &compileOptionsStream);
	addStream("linkOptions", &linkOptionsStream);	
	addStream("mainClose", &mainClose);

	// Initialize type conversion table
	typeConversionTable = cgcCnvTable;
	typeConversionTableRows = 7;
}

StringList CGCTarget::comment(const char* text, const char* b,
    const char* e, const char* c)
{
    const char* begin = "/* ";
    const char* end = " */";
    const char* cont = "   ";
    if (b != NULL) {
	begin = b; 
	end = e;
    	cont = c;
    }
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
    // States must be declared after the Star has been run because running
    // the Star builds the list of referenced States.

    globalDecls << star->declareStates(A_GLOBAL);
    globalDecls << star->declarePortHoles(P_GLOBAL);

    MAINDECLS << star->declareStates(A_LOCAL);
    MAINDECLS << star->declarePortHoles(P_LOCAL);

    cmdargStruct << star->cmdargStates();
    cmdargStructInit << star->cmdargStatesInits();	      
    setargFunc << star->setargStates();				
    setargFuncHelp << star->setargStatesHelps();		
    
    // States must be initialized before they are used, so this code must
    // appear before other code generated by the Star.
    prepend(star->initCodeStates(), MAININIT);
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
	cmdargStruct.initialize();	
	cmdargStructInit.initialize();	
	setargFunc.initialize();	
	setargFuncHelp.initialize();	
      }

StringList CGCTarget::pragma(const char* parentname,
			     const char* blockname) const {
    InfString compoundname;
    compoundname << parentname << "." << blockname;
    const char* value = mappings.lookup(compoundname);
    if (value) {
	StringList ret = "state_name_mapping ";
	ret += value;
	return ret;
    } else {
	return "";
    }
  }

StringList CGCTarget::pragma (const char* parentname,		
			      const char* blockname,
			      const char* pragmaname) const {
  InfString compoundname;
  compoundname << parentname << "." << blockname;
  const char* c = "";
  if (strcmp(pragmaname,"state_name_mapping") != 0 ||
      !(c = mappings.lookup(compoundname))) {
    c = "";
  }
  return c;
}

StringList CGCTarget::pragma (const char* parentname,		
			      const char* blockname,
			      const char* pragmaname,
			      const char* value) {
  InfString compoundname;
  compoundname << parentname << "." << blockname;
  if (strcmp(pragmaname,"state_name_mapping") == 0) {
    mappings.insert(compoundname, value);
  }
  return "";
}

static char* complexDecl =
"\n#if !defined(COMPLEX_DATA)\n#define COMPLEX_DATA 1"
"\n typedef struct complex_data { double real; double imag; } complex; \n"
"#endif\n";
static char* fixDecl =
"\n#if !defined(NO_FIX_SUPPORT)"
"\n/* include definitions for the fix runtime library */"
"\n#include \"CGCrtlib.h\""
"\n#endif\n";
static char* stdlibDecl =					
"\n#include <stdlib.h>\n";					

static char* setargHead =					
"void set_arg_val(char *arg[]) {\n"				
"\tint i;\n"							
"\tfor(i = 1; arg[i]; i++) {\n"					
"\t\tif((!strcmp(arg[i], \"-help\")) \\\n|| (!strcmp(arg[i], \"-HELP\")) "
"\\\n|| (!strcmp(arg[i], \"-h\"))) {\n"				
"\t\t\tprintf(\"Settable states are :\\\n\\n";			


void CGCTarget::trailerCode()
{
    include << stdlibDecl << fixDecl << complexDecl;		

    if (galaxy() && !SimControl::haltRequested()) {
        declareGalaxy(*galaxy());
	HLLTarget::trailerCode();
    }
}

// Initial stage of code generation.
void CGCTarget::headerCode()
{
    // Do nothing.
}

// Combine all sections of code.
void CGCTarget :: frameCode ()
{
    // Construct the header.  Include directives appear before any other
    // code.  All global variable declarations must appear before any code
    // (such as procedures) which may use those variables.  Within the
    // main function, declarations must appear before any code.

    StringList code = headerComment();
    code << include << globalDecls << procedures;

    // If there are command-line settable states in the target,
    // add the supporting code.
    if ((cmdargStruct.length() != 0)) {				
      code << "\nstruct {\n" << cmdargStruct			
    	   << "} arg_store = {" << cmdargStructInit		
    	   << "};\n\n"						
								
    	   << setargHead << setargFuncHelp			
    	   << "\");\n\t\t\texit(0);\n\t\t}\n"			
    	   << setargFunc << "\t}\n}\n\n";			
    }								

    code << comment("main function")				
	 << (const char*)funcName << "(int argc, char *argv[]) {\n"
	 << mainDecls;							
    if ((cmdargStruct.length() != 0))					
      code << "set_arg_val(argv);\n";					
    code << commInit << mainInit;					

    // Prepend the header.
    prepend(code, myCode);

    // Append the trailer.
    myCode << mainClose << "\n}\n";

    // after generating code, initialize code strings again.
    initCodeStrings();
}

void CGCTarget :: writeCode()
{
    writeFile(myCode,".c",displayFlag);
    if (!onHostMachine(targetHost)) {
	const char* header = "$PTOLEMY/src/domains/cgc/rtlib/CGCrtlib.h";
	const char* source = "$PTOLEMY/src/domains/cgc/rtlib/CGCrtlib.c";
	rcpCopyFile(targetHost, destDirectory, header);
	rcpCopyFile(targetHost, destDirectory, source);
    }
}

int CGCTarget::compileCode()
{
    // invoke the compiler
    StringList file, cmd, error, rtlib;
    if (onHostMachine(targetHost)) {
	StringList ptolemy;
	ptolemy << getenv("PTOLEMY");
	rtlib << "-I" << ptolemy << "/src/domains/cgc/rtlib -L" 
	      << ptolemy << "/lib." << getenv("PTARCH") << " -lCGCrtlib" ;
    }
    else {
	rtlib << "CGCrtlib.c";
    }
    file << filePrefix << ".c " << rtlib;
    cmd << compileLine(file) << " -o " << filePrefix;
    error << "Could not compile " << file;
    return (systemCall(cmd, error, targetHost) == 0);
}

// return compile command
StringList CGCTarget :: compileLine(const char* fName) {
	StringList cmd = (const char*) compileCommand;
	cmd << " " << (const char*) compileOptions << " "
	    << compileOptionsStream
	    << fName << " " << (const char*) linkOptions
	    << " " << linkOptionsStream;
	return cmd;
}

// Run the code.
int CGCTarget :: runCode()
{
    StringList cmd, error;
    cmd << "./" << filePrefix << "&";
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
	    	   << targetNestedSymbol.get() << "++) {\n";
 	    targetNestedSymbol.pop();
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
	if (! galaxy()) return FALSE;

	Galaxy& g = *galaxy();
	// set up the forkDests members of each Fork inputs.
	setupForkDests(g);
	
	int statBuf = useStaticBuffering();
	if (int(loopingLevel)) statBuf = 0;

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
				StringList sym, sName;
				sName << sanitizedName(*p);
				sym << symbol(sName);
				p->setGeoName(sym);
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
	if (! galaxy()) return FALSE;

	// call initialization code.
	defaultStream = &MAININIT;
	GalStarIter nextStar(*galaxy());
	CGCStar* s;
	while ((s = (CGCStar*) nextStar++) != 0) {
		if (s->isItFork()) continue;

		// Generate PortHole initialization code before the Star is run
		// because running the Star modifies the PortHole indices.
		MAININIT << s->initCodePortHoles();
		s->initCode();

	}

        return TRUE;
}

// Splice in stars.
// Return FALSE on error.

int CGCTarget::needsTypeConversionStar(PortHole& port) {
    if (HLLTarget::needsTypeConversionStar(port)) return TRUE;
    // check for ports of type FIX;
    // splice a FixToFix star if the precisions do not match or
    // one or both side uses precision variables that can change at
    // runtime
    if (port.isItOutput() && port.resolvedType() == FIX) {

	Precision p1, p2;

	// Determine the two connected ports;
	// If this port is the output of a fork star we go back to
	// the input of the fork
	CGCPortHole* far_port  = (CGCPortHole*)port.far();
	CGCPortHole* this_port = far_port->realFarPort();

	// This is a hack - we really need to clean up these casts.
	// If this or the far star is a wormhole, and the port
	// associated to the wormhole is a multiport - 
	// the multiport will be a WormMultiPort not a MultiCGCPort.
	// In this case, we assume that we do need a type conversion star.
	// FIXME
	if (far_port->parent()->isItWormhole() ||
	    this_port->parent()->isItWormhole())
	    return TRUE;
	
	// if this or the far side is a multiporthole, refer to the
	// multiport rather than to the normal porthole itself;
	// this is because when deriving a new port from the multiport,
	// the precision of the multiport may not have been set yet
	// (cf. MultiInCGCPort::newPort)

	MultiCGCPort *this_mph,*far_mph;
	this_mph = (MultiCGCPort*)this_port->getMyMultiPortHole();
	far_mph  = (MultiCGCPort*)far_port ->getMyMultiPortHole();

	// Get the precision assigned to the two connected ports.
	// Since the precision() method eventually passes the
	// precision of the connected port, we use validPrecision()
	// to determine whether a precision has been assigned
	// explicitly to the port.

	if (this_mph && this_mph->validPrecision())
	    p1 = this_mph->precision();
	else if (this_port->validPrecision())
	    p1 = this_port->precision();

	if (far_mph && far_mph->validPrecision())
	    p2 = far_mph->precision();
	else if (far_port->validPrecision())
	    p2 = far_port->precision();

	// if at least one of the connected ports uses variable
	// precisions and the other side defines its own precision,
	// we need a FixToFix star
	if (((this_port->attributes() & AB_VARPREC) && p2.isValid()) ||
	    ((far_port->attributes()  & AB_VARPREC) && p1.isValid()))
	    return TRUE;

	// otherwise check whether the precisions differ
	if (p1.isValid() && p2.isValid() && (p1 != p2))
	    return TRUE;
    }
    return FALSE;
}

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
	if (! galaxy()) return;
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
		defaultStream = &MYCODE;
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
			StringList sym, sName;
			sName << sanitizedName(*p);
			sym << symbol(sName);
			p->setGeoName(sym);
	    }
	}

	// Generate PortHole initialization code before the Star is run
	// because running the Star modifies the PortHole indices.
	MAININIT << cs->initCodePortHoles();

	defaultStream = &MAININIT;
	cs->initCode();

	// run the star
	defaultStream = &MYCODE;
	writeFiring(*cs, 1);

	defaultStream = &MAINCLOSE;
	cs->wrapup();
	defaultStream = &myCode;

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
	defaultStream = &MYCODE;
	s->compileRun();
	defaultStream = &MAINCLOSE;
}

/////////////////////////////////////////
// Utilities
/////////////////////////////////////////

ISA_FUNC(CGCTarget,HLLTarget);
