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

 Programmer: Jose Luis Pino

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGCPortHole.h"
#include "CGCTargetWH.h"
#include "KnownTarget.h"
#include "KnownBlock.h"
#include "Linker.h"
#include "InterpGalaxy.h"
#include "miscFuncs.h"
#include "CGCSDFBase.h"
#include "CGCSDFReceive.h"
#include "CGCSDFSend.h"

CGCTargetWH::CGCTargetWH(const char* name,const char* starclass, const
			 char* desc) : CGCTarget(name,starclass,desc) {
			     addStream("starPorts",&starPorts);
			     addStream("starSetup",&starSetup);
			     dirty = 0;
}

/*virtual*/ void CGCTargetWH::setup () {
    if (dirty) {
	StringList message;
	message << "Last time that the SDF-CGC wormhole galaxy "
		<< galaxy()->name() << " was compiled, the graph "
		<< "was left in a disconnected state because it "
		<< "failed to compile.  Edit the graph and correct "
		<< "the previously reported errors.";
	Error::abortRun((const char*) message);
	return;
    }
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

void commStarInit(CGCSDFBase* s,PortHole& p,int numXfer) {
    s->sdfPortName = p.name();
    s->numXfer = numXfer;
    DataType type = p.newConnection().resolvedType();
    s->sdfPortType = type;
}

CommPair cgcIncoming(PortHole& p,int numXfer) {
    CommPair pair;
    pair.outer = NULL;
    LOG_NEW; CGCSDFBase *newStar = new CGCSDFReceive;
    commStarInit(newStar,p,numXfer);
    pair.inner = newStar;
    return pair;
}

CommPair cgcOutgoing(PortHole& p,int numXfer) {
    CommPair pair;
    pair.outer = NULL;
    LOG_NEW; CGCSDFBase *newStar = new CGCSDFSend;
    commStarInit(newStar,p,numXfer);
    pair.inner = newStar;
    return pair;
}

/*virtual*/ void CGCTargetWH::wormPrepare() {
    dirty = 1;
    prepareCGCWorm(wormInputStars,wormOutputStars,
		   &cgcIncoming,&cgcOutgoing,*galaxy());
}

/*virtual*/ int CGCTargetWH::prepareCGCWorm(
    SDFSchedule &inputs, SDFSchedule &outputs,
    CommPairF incoming, CommPairF outgoing, Galaxy& gal)
{
    BlockPortIter nextPort(gal);
    DFPortHole *p;
    while ((p = (DFPortHole*)nextPort++) != 0) {
	// Must save type *before* disconnecting port
	CGStar* newStar;
	CGPortHole &cgPort = (CGPortHole&)p->newConnection();
	int numXfer=((DataFlowStar*)cgPort.parent())->reps()*cgPort.numXfer(); 
	cgPort.disconnect();
	DFPortHole *source,*destination,*newPort;
	StringList newStarName;
	if (p->isItInput()) {
	    CommPair inPair = (*incoming)(*p,numXfer);
	    newStar = inPair.inner;
	    newStarName << symbol("WormSend");
	    inputs.append(*newStar);
	    newPort = source = (DFPortHole*)newStar->portWithName("output");
	    destination = &cgPort;
	}
	else {
	    CommPair outPair = (*outgoing)(*p,numXfer);
	    newStar = outPair.inner;
	    newStarName << symbol("WormReceive");
	    outputs.append(*newStar);
	    source = &cgPort;
	    newPort =destination=(DFPortHole*)newStar->portWithName("input");
	}
	const char* wormName = hashstring(newStarName);
	gal.addBlock(*newStar,wormName);
	newStar->setTarget(this);
	source->connect(*destination,0);
	newStar->initialize();
	newStar->repetitions = 1;
    }
    nextPort.reset();
    return !SimControl::haltRequested();
}

/*virtual*/ int CGCTargetWH::compileCode() {
    StringList command;
    command << "cd " << (const char*)destDirectory << ";"
	    << "make -f " << (const char*) filePrefix << ".mk "
	    << (const char*) filePrefix << ".o";
    if(system(command)) {
	Error::abortRun("Compilation error");
	return FALSE;
    }
    return TRUE;
}

/*virtual*/ int CGCTargetWH::loadCode() {
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
    dirty = 0;
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

/*virtual*/ void CGCTargetWH::frameCode() {
    StringList code;
    code << "defstar{\n\tname{ "<< galaxy()->name() 
	 <<"}\n\tdomain{SDF}\n\tdesc{\n"
	 << headerComment() <<"\t}\n\tlocation{ "<<(const char*)destDirectory 
	 << "}\n"<<starPorts<< "\n\theader{\n" << include << fixDecl 
	 << complexDecl << globalDecls << procedures << "\t}\nprivate{\n"
	 << mainDecls << "\t}\n\tbegin{\n" << commInit << mainInit 
	 << "\t}\n\tsetup{\n" << starSetup
	 << "\t}\n\tgo{\n" << myCode
	 << "\t}\n\twrapup{\n" << mainClose << "\t}\n}";
    myCode.initialize();
    myCode << code;
    initCodeStrings();
}

/*virtual*/ void CGCTargetWH::writeCode() {
    writeFile(myCode,".pl",displayFlag);
    // Construct makefile
    StringList makefile;
    makefile << headerComment("# ") << "# To make the star, do: " 
	     << "make -f " << (const char*) filePrefix << ".mk "
	     << (const char*) filePrefix << ".o\n\n" 
	     << "ROOT = " << expandPathName("$PTOLEMY") << "\n"
	     << "VPATH = .\n"
	     << "CONFIG=$(ROOT)/mk/config-$(ARCH).mk\n"
	     << "include $(CONFIG)\n"
	     << "SDFDIR = $(ROOT)/src/domains/sdf/kernel\n"
	     << "CGCDIR = $(ROOT)/lib/cgc\n"
	     << "INCL = -I$(SDFDIR) -I$(KERNDIR) -I$(CGCDIR) "
	     << (const char *) compileOptions << "\n" 
	     << "include $(ROOT)/mk/common.mk\n";
    writeFile(makefile,".mk");
}

/*virtual*/ void CGCTargetWH::initCodeStrings() {
    CGCTarget::initCodeStrings();
    starPorts.initialize();
}

/*virtual*/ int CGCTargetWH::run () {
    return TRUE;
}

/*virtual*/ void CGCTargetWH::mainLoopCode() {
    defaultStream = &myCode;
    allWormInputCode();
    compileRun((SDFScheduler*) scheduler());
    CodeStream* crStream = defaultStream;
    defaultStream = &myCode;
    allWormOutputCode();
    defaultStream = crStream;
}

/*virtual*/ void CGCTargetWH::allWormInputCode() {
    SDFSchedIter nextStar(wormInputStars);
    DataFlowStar* star;
    while ((star = nextStar++) != 0)
	writeFiring(*star,1);
}

/*virtual*/ void CGCTargetWH::allWormOutputCode() {
    SDFSchedIter nextStar(wormOutputStars);
    DataFlowStar* star;
    while ((star = nextStar++) != 0)
	writeFiring(*star,1);
}

/*virtual*/ void CGCTargetWH::wrapup () {
    // delete galaxy();
}

Block* CGCTargetWH::makeNew () const {
    LOG_NEW; return new CGCTargetWH(name(),starType(),descriptor());
}

ISA_FUNC(CGCTargetWH,CGCTarget);
static CGCTargetWH targ("CGCTargetWH","CGCStar","Wormhole target for CGC.");
static KnownTarget entry(targ,"CGCTargetWH");
int CGCTargetWH::linkFiles ()
{
    // link the object file into a ptolemy simulation
    StringList silPlFname,ss;
    silPlFname << (const char*)destDirectory;
    silPlFname << "/" << (const char*) filePrefix << ".o" ;
    ss = expandPathName(silPlFname);
    int status = Linker::linkObj(ss); 
    return status;
}

int CGCTargetWH::connectStar(Galaxy& ggal) {
    Galaxy* parentGal = (Galaxy*)ggal.parent()->parent();
    StringList	starname;
    starname << galaxy()->name() << "Worm";

    Block *newWormStar = KnownBlock::clone(galaxy()->name(),"SDF");
    if (newWormStar == 0) {
	StringList message;
	message << "Can not clone the newly linked SDF worm star: " 
		<< galaxy()->name();
	Error::abortRun((const char*)message);
	return FALSE;
    }
    parentGal->addBlock(*newWormStar,starname);

    Block* oldWormHole = ggal.parent();
    PortHole* wormPort;
    BlockPortIter wpNext(*oldWormHole);
    while ((wormPort = wpNext++) != 0 ) {
	PortHole* farPort = wormPort->far();
	int delays = wormPort->numInitDelays();
	PortHole* newPort =
	    newWormStar->portWithName(wormPort->name());
	farPort->disconnect();
	newPort->connect(*farPort,delays);
    }
    
    parentGal->deleteBlockAfterInit(*oldWormHole);
    return TRUE;

}

















