static const char file_id[] = "CGCTargetWH.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1995  The Regents of the University of California.
All Rights Reserved.

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

 Programmer: Jose Luis Pino, initial version based on SIlageSimTarget

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCPortHole.h"
#include "CGCTargetWH.h"
#include "KnownTarget.h"
#include "Linker.h"
#include "InterpGalaxy.h"
#include "miscFuncs.h"

CGCTargetWH::CGCTargetWH(const char* name,const char* starclass, const
			 char* desc) : CGCTarget(name,starclass,desc) { 
			     addStream("starPorts",&starPorts);
}

void CGCTargetWH :: setup () {
    if(galaxy()->parent() == NULL) {
	Error::abortRun("CGCTargetWH runs only inside of SDF");
	return;
    }

    StringList plPrefix;
    plPrefix << "SDF" << galaxy()->name(); 
    filePrefix = savestring(plPrefix);
    CGCTarget::setup();

    if(Scheduler::haltRequested()) return ;

}

int CGCTargetWH::compileCode() {
    // compile the pl to a cc file
    fprintf(stderr,"Compiling the .pl file to a .cc file\n");
    if(!makeCCFile())
	{Error::message("ptlang failed"); return FALSE;}

    // compile the cc code 
    fprintf(stderr,"Compiling the .cc file to a .o file\n");
    if(!compileCCCode())
	{Error::message("compile failed"); return FALSE;}

    return TRUE; 
}

int CGCTargetWH::loadCode() {
    fprintf(stderr,"Linking in the new .o file\n");
    if(!linkFiles()) {
	Error::abortRun("Failed to link in new star, aborting");
	return 0;
    }
    fprintf(stderr,"replacing CGC wormhole by the new star\n");
    if(!connectStar(*galaxy())) {
	Error::abortRun("Failed to connect new star, aborting");
	return 0;
    }
    return 1;
}
// CODE DUPLICATION FROM CGCTarget

static char* complexDecl =
"\n#if !defined(COMPLEX_DATA)\n#define COMPLEX_DATA 1"
"\n typedef struct complex_data { double real; double imag; } complex; \n"
"#endif\n";
static char* fixDecl =
"\n#if !defined(NO_FIX_SUPPORT)"
"\n/* include definitions for the fix runtime library */"
"\n#include \"CGCrtlib.h\""
"\n#endif\n";

void CGCTargetWH::frameCode() {
    StringList code;
    code << "defstar{\n\tname{ "<< galaxy()->name() 
	 <<"}\n\tdomain{SDF}\n\tdesc{\n"
	 << headerComment() <<"\t}\n\tlocation{ "<<(const char*)destDirectory 
	 << "}\n"<<starPorts<< "\n\theader{\n" << include << fixDecl 
	 << complexDecl << globalDecls << procedures << "\t}\nprivate{\n"
	 << mainDecls << "\t}\n\tbegin{\n" << commInit << mainInit 
	 << "\t}\n\tgo{\n" << myCode
	 << "\t}\n\twrapup{\n" << mainClose << "\t}\n}";
    myCode.initialize();
    myCode << code;
    initCodeStrings();
}

void CGCTargetWH::writeCode() {
    writeFile(myCode,".pl",displayFlag);
}

void CGCTargetWH::initCodeStrings() {
    CGCTarget::initCodeStrings();
    starPorts.initialize();
}

int CGCTargetWH :: run () {
//    Galaxy* newGal = galaxy();
//    GalTopBlockIter newIter(*newGal);
//    Block* star;
//    const char* onm = galaxy()->name();

// run the star added that replaces the galaxy. This star is called wormName.

/*    while ((star = newIter++) != 0)
   	{
	    const char* snm = star->name();
	    if( strcmp(snm,onm) == 0) break;
   	}

    SilageStar* silStar;
    silStar = (SilageStar*)star;
    SilageStarPortIter nextStarPort(*silStar);
    SilagePortHole* silPh;
    for(int i = star->numberPorts(); i > 0; i--)
	{
	    silPh = nextStarPort++;
	    silPh->receiveData();
	}
    if( !silStar->runSim() ) return FALSE;
    nextStarPort.reset();
    for(i = star->numberPorts(); i > 0; i--)
	{
	    silPh = nextStarPort++;
	    silPh->sendData();
	} */
    return TRUE;
}

/* virtual */ void CGCTargetWH::wormInputCode(PortHole& p) {
    CGCPortHole& cp = (CGCPortHole&)p;
    starPorts << "\tinput {\n\t\tname{" << cp.name() << "}\n\t\ttype{";
    if (strcmp(cp.resolvedType(),INT)==0) {
	starPorts << "int";
    }
    else if (strcmp(cp.resolvedType(),FLOAT)==0) {
	starPorts << "float";
    }
    else {
	StringList message;
	message << "WormInputCode: Port" << cp.parent()->name() << "." 
		<< cp.name() << " uses unsupported wormhole type of "
		<< cp. resolvedType();
	Error::abortRun(*this,message);
	return;
    }
    starPorts <<"}\n\t}\n";
    myCode << "for (int i = " << cp.numXfer() - 1 << "; i >=0 ; i--) {\n"
	   << "\tint j = 0;\n\t" << cp.getGeoName();
    if (cp.bufSize() > 1 || cp.bufType() == EMBEDDED) {
	myCode << '[';
	if (cp.staticBuf()) myCode << cp.bufPos();
	else {
	    StringList bufPtr;
	    bufPtr << sanitize(cp.name()) << "Counter";
	    mainDecls << "int " << bufPtr << " = 0;\n";
	    myCode << "bufPtr";
	}
	myCode << " + j++]";
    }
    myCode << " = " << cp.name() << "%i;\n}\n";
}

/* virtual */ void CGCTargetWH::wormOutputCode(PortHole& p) {
    CGCPortHole& cp = (CGCPortHole&)p;
    starPorts << "\tinput {\n\t\tname{" << cp.name() << "}\n\t\ttype{";
    if (strcmp(cp.resolvedType(),INT)==0) {
	starPorts << "int";
    }
    else if (strcmp(cp.resolvedType(),FLOAT)==0) {
	starPorts << "float";
    }
    else {
	StringList message;
	message << "WormOutputCode: Port" << cp.parent()->name() << "." 
		<< cp.name() << " uses unsupported wormhole type of "
		<< cp. resolvedType();
	Error::abortRun(*this,message);
	return;
    }
    starPorts <<"}\n\t}\n";
    myCode << "for (int i = " << cp.numXfer() - 1 << "; i >=0 ; i--) {\n"
	   << "\tint j = 0;\n\t" << cp.name() << "%i = " << cp.getGeoName();
    if (cp.bufSize() > 1 || cp.bufType() == EMBEDDED) {
	myCode << '[';
	if (cp.staticBuf()) myCode << cp.bufPos();
	else {
	    StringList bufPtr;
	    bufPtr << sanitize(cp.name()) << "Counter";
	    mainDecls << "int " << bufPtr << " = 0;\n";
	    myCode << "bufPtr";
	}
	myCode<< "+ j++]";
    }
    myCode << ";\n}\n";
}

void CGCTargetWH :: wrapup () {
    // delete galaxy();
}

Block* CGCTargetWH :: makeNew () const {
    LOG_NEW; return new CGCTargetWH(name(),starType(),descriptor());
}

ISA_FUNC(CGCTargetWH,CGCTarget);
static CGCTargetWH targ("CGCTargetWH","CGCStar","Wormhole target for CGC.");
static KnownTarget entry(targ,"CGCTargetWH");

int CGCTargetWH :: makeCCFile () {
    // this compiles <>.pl to a SDF<>.{h,cc}
    StringList ptlangOptions = "";
    ptlangOptions << "cd " << (const char*)destDirectory << ";" ;
    ptlangOptions << "ptlang " << (const char*) filePrefix << ".pl;"  ;
    if(system(ptlangOptions)) {Error::abortRun("errors in ptlang");
    return FALSE; }
    return TRUE;
}

int CGCTargetWH :: compileCCCode () {
    // compile the .pl file 
    const char* ptolemyRoot;
    ptolemyRoot = getenv ("PTOLEMY");
    if (ptolemyRoot == 0) ptolemyRoot = expandPathName("~ptolemy");

    StringList ccOptions = "";
    ccOptions << "cd " << (const char*)destDirectory << ";" ;
    ccOptions << "g++ -g -w ";

    ccOptions << "-DBITTRUE -DDETOV ";
    ccOptions << "-I" << ptolemyRoot << "/src/domains/cgc/stars";
    ccOptions << " -I" << ptolemyRoot << "/src/domains/cgc/kernel";
    ccOptions << " -I" << ptolemyRoot << "/src/domains/cg/kernel";
    ccOptions << " -I" << ptolemyRoot << "/src/domains/sdf/kernel";
    ccOptions << " -I" << ptolemyRoot << "/src/kernel";
    ccOptions << " -I" << ptolemyRoot << "/vendors/silage_support/include";
    ccOptions << " -c " << (const char*) filePrefix << ".cc";

    if(system(ccOptions)) 
	{Error::abortRun("Compilation error"); return FALSE; }
    return TRUE;
}

int CGCTargetWH :: linkFiles ()
{
    // link the object file into a ptolemy simulation
    StringList silPlFname,ss;
    silPlFname << (const char*)destDirectory;
    silPlFname << "/" << (const char*) filePrefix << ".o" ;
    ss = expandPathName(silPlFname);
    int status = Linker::linkObj(ss); 
    return status;
}

int CGCTargetWH :: connectStar(Galaxy& ggal) {
    const char*	sname;
    const char*	sclass;

    sclass = sname = (const char*) filePrefix; 

    int status = ((InterpGalaxy*)&ggal)->addStar(sname, sclass);
    if(!status) return FALSE;

    Block* bs3;
    GalTopBlockIter BSnext(ggal);
    // get the name of the star to connect to wormhole boundary 
    while ((bs3 = BSnext++) != 0) { 
	const char* snm3 = bs3->name();
	if( strcmp(snm3,sname) == 0) break;
    }
	
    Block* bb;
    GalTopBlockIter BBnext(ggal);
    while ((bb = BBnext++) != 0) { 
	const char* snm3 = bb->name();
	if( strcmp(snm3,sname) == 0) break;
    }

    const char* galPhName;
    const char* starPhName;

    BlockPortIter galPortIter(ggal);
    BlockPortIter gIter(ggal);
    PortHole* ph;
    while ((ph = galPortIter++) != 0) {
	// connect galaxy portholes to the star portholes
	PortHole& tph = *gIter++;
	PortHole& rph = (PortHole&) tph.realPort();
	
	galPhName = ph->name();	
	PortHole* wormPh;
	PortHole* aliasPh;
	aliasPh = (PortHole*)&(rph);
	wormPh = (PortHole*)aliasPh->far();
	aliasPh->far()->disconnect();
	BlockPortIter phIter(*bs3);
	CGCPortHole* bsph; 
	while ((bsph =(CGCPortHole*)phIter++) != 0) {
	    starPhName = bsph->name();
	    if( strcmp(galPhName,starPhName) == 0) {
		ph->setAlias(*bsph);
		bsph->connect(*wormPh,0);
	    } // if
	} // while
    } // while

    GalTopBlockIter next(ggal);
    Block* b;
    const char* blkName;
    while ((b = next++) != 0 ) {
	blkName = b->name();
	if(b->isItAtomic()) {
	    if ( strcmp(blkName,sname) == 0) {
		b->initialize();
		break;		
	    } // initialized added star
	} // if atomic
    } // while
    return TRUE;
}
