static const char file_id[] = "CGCTargetWH.cc";
/******************************************************************
Version identification:
$Id$

Copyright (c) 1991-%Q%  The Regents of the University of California.
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

#include "CGUtilities.h"
#include "Domain.h"
#include "CGCPortHole.h"
#include "CGCTargetWH.h"
#include "KnownTarget.h"
#include "KnownBlock.h"
#include "Linker.h"
#include "InterpGalaxy.h"
#include "PortHole.h"
#include "miscFuncs.h"
#include "CGCSDFReceive.h"
#include "CGCSDFSend.h"
#include "DynamicGalaxy.h"
#include "Geodesic.h"

CGCTargetWH::CGCTargetWH(const char* name,const char* starclass, const
			 char* desc) : CGCTarget(name,starclass,desc) {
			     addStream("starPorts",&starPorts);
			     addStream("starSetup",&starSetup);
			     dirty = 0;
			     wormhole = 0;
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

    // Create the base name for the SDF star to be generated
    StringList plPrefix;
    plPrefix << "SDF" << galaxy()->name(); 
    filePrefix = savestring(plPrefix);

    // Create the replacement SDF worm star
    CGCTarget::setup();

    if(Scheduler::haltRequested()) return ;

}

void commStarInit(CGCSDFBase* s,PortHole& p,int numXfer) {
    s->sdfPortName = p.name();
    s->numXfer = numXfer;
    DataType type = p.newConnection().resolvedType();
    s->sdfPortType = type;
}

// Create the CGC star that receives data from simulation
CommPair cgcIncoming(PortHole& p,int numXfer, CGTarget& target) {
    CommPair pair;
    pair.outer = NULL;
    LOG_NEW; CGCSDFBase *newStar = new CGCSDFReceive;
    commStarInit(newStar,p,numXfer);
    pair.inner = newStar;
    return pair;
}

// Create the CGC star that sends data to simulation
CommPair cgcOutgoing(PortHole& p,int numXfer, CGTarget& target) {
    CommPair pair;
    pair.outer = NULL;
    LOG_NEW; CGCSDFBase *newStar = new CGCSDFSend;
    commStarInit(newStar,p,numXfer);
    pair.inner = newStar;
    return pair;
}

// Splice in the CGC-SDF communication actors
/*virtual*/ void CGCTargetWH::wormPrepare() {
    dirty = 1;
    prepareCGCWorm(wormInputStars,wormOutputStars,
		   &cgcIncoming,&cgcOutgoing,*this);
}

// FIXME, modifyGalaxy should always work.  It does not 
// need to be run with the current cg56-cgc wormholes... but
// this does not mean it never has to be run.  The problem is that
// in case 2 below, CGC comm stars are first connected to the non-CGC
// event horizions.  A correct solution would be to have a SDF-CGC wormhole
// replace the SDF-CG56 wormhole.

/*virtual*/ int CGCTargetWH::modifyGalaxy() {
    // If the parent of this galaxy is not a wormhole, this modifyGalaxy is
    // being called by the setup method of CGCTarget.  This target is
    // being used as a helper target.  Only call modifyGalaxy when this target
    // is NOT being used as a helper target.
    if(galaxy()->parent()->isItWormhole())
	return CGCTarget::modifyGalaxy();
    return TRUE;
}

// This is the MAIN method, it is called both from this target
// and from targets using this target as a helper target.
//
// Case 1: Stand-alone target
//         Create a CGC-SDF communication star for each wormhole input and 
//         output port.
// Case 2: As a helper target
//         Called first to splice in communication star pairs for each wormhole
//         porthole.  This will communicate data b/w the original target and
//         CGC.  The CGC communication stars are collected in the galaxy 
//         owned by CGCTargetWH.  Finally, the CGCTargetWH is called by
//         the original target (setup), which runs this code as Case 1 to
//         create communication to SDF.

/*virtual*/ int CGCTargetWH::prepareCGCWorm(
    SDFSchedule &inputs, SDFSchedule &outputs,
    CommPairF incoming, CommPairF outgoing, CGTarget& tar)
{
    Galaxy& gal = *tar.galaxy();

    // Check to make sure we are inside a wormhole.  If so, log the wormhole
    // pointer (so we won't loose it in the second call to this method, case 2)
    if (!wormhole) {
	if(gal.parent()->isItWormhole()) 
	    wormhole = gal.parent();
	else {
	    Error::abortRun(tar,"Is not inside a wormhole");
	    return FALSE;
	}
    }

    // If the calling target is not this one, we must setup this
    // target.
    if (&tar != this) {
	// First we create the galaxy
	DynamicGalaxy* cgcGalaxy = new DynamicGalaxy;
	StringList cgcWormName;
	cgcWormName << wormhole->name() << "CGC";
	cgcGalaxy->setName(hashstring(cgcWormName));
	setGalaxy(*cgcGalaxy);

	// Next we create the scheduler, this will never actually 
	// schedule the graph, but rather have a schedule (for the
	// cgc comm actors) written to it by CGTarget::copySchedule
	SDFScheduler* sched = new SDFScheduler;
	sched->setGalaxy(*cgcGalaxy);
	setSched(sched);

	// Set the generated code destination directory
	destDirectory = tar.destDirectory;

	// Determine galaxy that contains this wormhole
	Galaxy& parentGal = *(Galaxy*) gal.parent()->parent();

	// Create a wormhole for the parent domain that will contain this
	// CGCTargetWH and its galaxy.  This is a easy method to insure that
	// both the target, and all the stars in the galaxy are properlly 
	// destroyed
	Block& wormXXXCGC = 
	    Domain::named(parentGal.domain())->newWorm(*galaxy(),this);

	// Add the wormhole to the parent galaxy
	parentGal.addBlock(wormXXXCGC,cgcGalaxy->name());

	// log it for deletion after the initialize block phase.  It will
	// be deleted by Galaxy::initSubBlocks
	parentGal.deleteBlockAfterInit(wormXXXCGC);
    }

    // These schedules will only be used if this target is being used as
    // a helper target.  They collect all of the pair.outer stars.
    SDFSchedule cgcIn,cgcOut;
    cgcIn.initialize();
    cgcOut.initialize();

    // Iterate on all of the galaxy ports
    BlockPortIter nextPort(gal);
    DFPortHole *p;
    while ((p = (DFPortHole*)nextPort++) != 0) {
	// Must save type *before* disconnecting port
	CGPortHole &cgPort = (CGPortHole&)p->newConnection();
	CGPortHole &cgPortFar = *(CGPortHole*)cgPort.far();
	int numXfer=((DataFlowStar*)cgPort.parent())->reps()*cgPort.numXfer(); 
	int maxDelay= numXfer + cgPort.maxDelay()-cgPort.numXfer();
	cgPort.disconnect();
	DFPortHole *iSource,*iDestination,*iNew,*oSource,*oDestination,*oNew;

	// Create the communication pair for this port
	CommPair pair = (p->isItInput()? *incoming: *outgoing)(*p,numXfer,tar);

	if (p->isItInput()) {
	    inputs.append(*pair.inner);
	    iNew = iSource = (DFPortHole*)pair.inner->portWithName("output");
	    iDestination = &cgPort;
	    if (pair.outer) {
		cgcIn.append(*pair.outer);
		oSource = &cgPortFar;
		oNew = oDestination = 
		    (DFPortHole*)pair.outer->portWithName("input");
		if (iNew->type() == ANYTYPE) iNew->inheritTypeFrom(*oNew);
	    }
	}
	else {
	    outputs.append(*pair.inner);
	    iSource = &cgPort;
	    iNew=iDestination = (DFPortHole*)pair.inner->portWithName("input");
	    if (pair.outer) {
		cgcOut.append(*pair.outer);
		oNew =oSource= (DFPortHole*)pair.outer->portWithName("output");
		oDestination = &cgPortFar;
		if (oNew->type() == ANYTYPE) oNew->inheritTypeFrom(*iNew);
	    }
	}

	// Name the inner comm star
	StringList innerName,outerName;
        innerName << symbol("wormInner");
	const char* iName = hashstring((const char*)innerName);

	// Add the star to the inner galaxy
	gal.addBlock(*pair.inner,iName);

	iSource->connect(*iDestination,0);
	iNew->setSDFParams(numXfer,maxDelay);

	// Set and initialize the new star
	pair.inner->setTarget(&tar);
	pair.inner->initialize();
	pair.inner->repetitions = 1;
	iNew->geo()->setMaxArcCount(numXfer);

	if (pair.outer) {
	    // Name the outer comm star
	    outerName << symbol("wormOuter");
	    const char* oName = hashstring((const char*) outerName);

	    // Add the star to the CGCTargetWH galaxy
	    galaxy()->addBlock(*pair.outer,oName);

	    oSource->connect(*oDestination,0);
	    oNew->setSDFParams(numXfer,maxDelay);
	    
	    DataType res = p->newConnection().setResolvedType();
	    DataType dType = res ? res : p->type();
	    if (oNew->isItMulti()) {
		LOG_NEW; GalMultiPort *gp = new GalMultiPort(*oNew);
                galaxy()->addPort(gp->setPort(p->name(),galaxy(),dType));
                gp->GenericPort::setAlias(*oNew);
	    }
	    else {
                LOG_NEW; GalPort *gp = new GalPort(*oNew);
                galaxy()->addPort(gp->setPort(p->name(),galaxy(),dType));
                gp->GenericPort::setAlias(*oNew);
	    }
            // Set and initialize the new star
	    pair.outer->setTarget(this);
	    pair.outer->initialize();
	    pair.outer->repetitions = 1;
	    oNew->geo()->setMaxArcCount(numXfer);
	}

    }
    nextPort.reset();

    // if we are computing this for an external target, we must
    // combine the outer comm star schedules and copy it to this target.
    if (&tar!= this) {
	SDFSchedIter nextStar(cgcOut);
	DataFlowStar* star;
	while((star = nextStar++) != 0)
	    cgcIn.append(*star);
	copySchedule(cgcIn);
    }
    return !SimControl::haltRequested();
}

/*virtual*/ int CGCTargetWH::compileCode() {
    StringList command;
    command << "cd " << (const char*)destDirectory << "; "
	    << "make -f " << (const char*) filePrefix << ".mk all";
    if(system(command)) {
	Error::abortRun("Compilation error");
	return FALSE;
    }
    return TRUE;
}

/*virtual*/ int CGCTargetWH::loadCode() {
    if(!linkFiles()) {
	Error::abortRun("Failed to link in new star, aborting");
	return 0;
    }
    fprintf(stderr,"replacing CGC wormhole by the new star\n");
    if(!connectStar()) {
	Error::abortRun("Failed to connect new star, aborting");
	return 0;
    }
    dirty = 0;
    return 1;
}

/*virtual*/ void CGCTargetWH::frameCode() {
    StringList code;
    code << "defstar{\n\tname{ "<< galaxy()->name() 
	 <<"}\n\tdomain{SDF}\n\tdesc{\n"
	 << headerComment() <<"\t}\n\tlocation{ "<<(const char*)destDirectory 
	 << "}\nhinclude {\"Error.h\",\"SimControl.h\"}\n" 
	 <<starPorts<< "\n\theader{\nextern \"C\" {\n" << include 
	 << "}\n" << globalDecls << procedures << "\t}\nprivate{\n"
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
	     << "make -f " << (const char*) filePrefix << ".mk all\n"
	     << "ROOT = " << getenv("PTOLEMY") << "\n"
	     << "WORM_INCL = " << (const char *) compileOptions 
	     << starIncludeDirs << "\n" 
	     << "all: " << (const char*) filePrefix << ".o\n"
	     << "include $(ROOT)/mk/cgworm.mk\n\n";
    writeFile(makefile,".mk");
}

/*virtual*/ void CGCTargetWH::initCodeStrings() {
    CGCTarget::initCodeStrings();
    starPorts.initialize();
}

/*virtual*/ int CGCTargetWH::run () {
    return TRUE;
}

// Redefine main loop so that we do not iterate.  The iteration will be
// controlled by calling the go method of the new SDF star
/*virtual*/ void CGCTargetWH::mainLoopCode() {
    defaultStream = &myCode;
    allWormInputCode();
    compileRun((SDFScheduler*) scheduler());
    CodeStream* crStream = defaultStream;
    defaultStream = &myCode;
    allWormOutputCode();
    defaultStream = crStream;
}

void fireSDFSchedule(SDFSchedule& sched, CGTarget& target) {
    SDFSchedIter nextStar(sched);
    DataFlowStar* star;
    while ((star = nextStar++) != 0)
	target.writeFiring(*star,1);
}

/*virtual*/ void CGCTargetWH::allWormInputCode() {
    fireSDFSchedule(wormInputStars,*this);
}

/*virtual*/ void CGCTargetWH::allWormOutputCode() {
    fireSDFSchedule(wormOutputStars,*this);
}

/*virtual*/ void CGCTargetWH::wrapup () {
    // Do nothing
}

Block* CGCTargetWH::makeNew () const {
    LOG_NEW; return new CGCTargetWH(name(),starType(),descriptor());
}

ISA_FUNC(CGCTargetWH,CGCTarget);
static CGCTargetWH targ("CGCTargetWH","CGCStar","Wormhole target for CGC.");
static KnownTarget entry(targ,"CGCTargetWH");

int CGCTargetWH::linkFiles ()
{
    StringList linkCmd, dir;

    dir << expandPathName((const char*) destDirectory);
    linkCmd << dir << "/" << (const char*) filePrefix << ".o " 
	    << "-L" << getenv("PTOLEMY") << "/lib." <<getenv("PTARCH")
	    << " -lCGCrtlib " << starLinkOptions;

    const char* argv[2];
    const char* multiLink = "multilink";

    // The link commands must be in the second argv location
    argv[0] = multiLink;
    argv[1] = hashstring(linkCmd);
    fprintf(stderr,"multiLink: %s\n",argv[1]);
    int status = Linker::multiLink(2,(char**) argv); 
    fprintf(stderr,"multiLink completed\n");
    return status;
}

int CGCTargetWH::connectStar() {
    Galaxy* parentGal = (Galaxy*)wormhole->parent();
    StringList	starname;
    starname << galaxy()->name() << "Worm";
    const char* sname = hashstring((const char*)starname);

    Block *newWormStar = KnownBlock::clone(galaxy()->name(),"SDF");
    if (newWormStar == 0) {
	StringList message;
	message << "Can not clone the newly linked SDF worm star: " 
		<< galaxy()->name();
	Error::abortRun((const char*)message);
	return FALSE;
    }
    parentGal->addBlock(*newWormStar,sname);

    PortHole* wormPort;
    BlockPortIter wpNext(*wormhole);
    while ((wormPort = wpNext++) != 0 ) {
	PortHole* farPort = wormPort->far();
	int delays = wormPort->numInitDelays();
	PortHole* newPort =
	    newWormStar->portWithName(wormPort->name());
	farPort->disconnect();
	newPort->connect(*farPort,delays);
    }
    
    // We must initialize this new star, the star is created during
    // the initialize method of the parent galaxy, thus the initilize
    // method is run on the original wormhole, and not this
    // dynamically linked in star.
    newWormStar->initialize();

    // Tell the parent galaxy to delete the original wormhole.
    parentGal->deleteBlockAfterInit(*wormhole);

    return TRUE;

}

















