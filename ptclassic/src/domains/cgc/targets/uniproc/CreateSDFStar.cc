static const char file_id[] = "CreateSDFStar.cc";
/******************************************************************
Version identification:
$Id$

@Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer: Jose Luis Pino

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CGUtilities.h"
#include "Domain.h"
#include "CGCPortHole.h"
#include "CreateSDFStar.h"
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


// A simple class to compute the repetitions on a SDF graph - we need
// this because a lot of the SDFScheduler methods are protected.
// This class duplicates some of the functionality of
// SDFScheduler::setup  FIXME

class Repetitions:public SDFScheduler {
public:
    Repetitions(Galaxy& gal) {
	invalid = FALSE;
	setGalaxy(gal);
        if (!checkConnectivity()) return;
	if(!prepareGalaxy()) return;
	if (!checkConnectivity()) return;
	if (!checkStars()) return;
	if(!repetitions()) return;
	adjustSampleRates();
    }
};

CreateSDFStar::CreateSDFStar(const char* name,const char* starclass, const
			 char* desc) : CGCTarget(name,starclass,desc) {
    addStream("starPorts", &starPorts);
    addStream("starSetup", &starSetup);
}

void CreateSDFStar::setup () {
    if (! galaxy()) return;
    topLevelGalaxy = galaxy();
    if (inWormHole()) {
      // Construct name of target star, with and without "SDF" prefix
      StringList plPrefix;
      plPrefix << "SDF" << topLevelGalaxy->name() << "_compiled";
      filePrefix = hashstring(plPrefix);    
      const char* compiledStarName = filePrefix;
      compiledStarName += 3;
      // Is the star already present and newer than the galaxy?
      long oldCompiledSN = KnownBlock::serialNumber(compiledStarName,"SDF");
      // check the galaxy's contents
      long galaxySN = topLevelGalaxy->maxContainedSerialNumber();
      // check the galaxy itself, in the person of its containing wormhole
      Block* topWorm = topLevelGalaxy->parent();
      if (topWorm) {
	long wormSN = KnownBlock::serialNumber(*topWorm);
	if (wormSN > galaxySN)
	  galaxySN = wormSN;
      }
      if (oldCompiledSN <= galaxySN) {
	// Need to (re) compile and link the galaxy.
	CGCTarget::setup();
      }
      // Now we can link the compiled star into the outer galaxy.
      if (! SimControl::haltRequested()) {
	fprintf(stderr,"replacing CGC wormhole by the compiled star\n");
	if(!connectStar()) {
	  Error::abortRun("Failed to connect new star, aborting");
	  return;
	}
      }
    } else {
      // Not in a wormhole .. let outer target have control
      StringList plPrefix;
      plPrefix << "SDF" << filePrefix; 
      filePrefix = hashstring(plPrefix);
      CGCTarget::setup();
    }
}

void CreateSDFStar::wormPrepare() {
    if (! galaxy()) return;
    convertWormholePorts(*galaxy());
}

int CreateSDFStar::convertWormholePorts(Galaxy& gal) {
    // FIXME - Won't work unless CGC is at top level
    if(!gal.parent()->isItWormhole()) return FALSE;
    
    Repetitions computeReps(gal);
    if (SimControl::haltRequested()) return FALSE;

    // Iterate on all of the galaxy ports
    BlockPortIter nextPort(gal);
    DFPortHole *p;
    while ((p = (DFPortHole*)nextPort++) != 0) {
	// Must save type *before* disconnecting port
	CGPortHole &cgPort = (CGPortHole&)p->newConnection();
	DataType type = cgPort.resolvedType();
	int numXfer =
	    ((DataFlowStar*)cgPort.parent())->reps()*cgPort.numXfer(); 
	int maxDelay= numXfer + cgPort.maxDelay()-cgPort.numXfer();
	cgPort.disconnect();
	DFPortHole *source,*destination,*newPort;
	CGCSDFBase *newStar;
	StringList nm;
	if (p->isItInput()) {
	    newStar = new CGCSDFReceive;
	    newPort = source = &((CGCSDFReceive*)newStar)->output;
	    destination = &cgPort;
	    nm << symbol("CGCSDFReceive");
	}
	else {
	    newStar = new CGCSDFSend;
	    source = &cgPort;
	    newPort = destination = &((CGCSDFSend*)newStar)->input;
	    nm << symbol("CGCSDFSend");
	}

	// Add the star to the inner galaxy
	gal.addBlock(*newStar,hashstring(nm));
	newStar->setTarget(this);
	newStar->setSDFPortInfo(hashstring(ptSanitize(p->name())),
				type, numXfer, maxDelay);
	source->connect(*destination,0);
    }
    return !SimControl::haltRequested();
}
		
int CreateSDFStar::compileCode() {
    StringList command;
    command << "cd " << (const char*) destDirectory << "; "
	    << "make -f " << (const char*) filePrefix << ".mk all";
    if (systemCall(command,"Compilation error")) return FALSE;
    return TRUE;
}

int CreateSDFStar::loadCode() {
    if(!linkFiles()) {
	Error::abortRun("Failed to link in new star, aborting");
	return FALSE;
    }
    return TRUE;
}

void CreateSDFStar::frameCode() {
    StringList code;
    const char* compiledStarName = filePrefix;
    compiledStarName += 3;
    code << "defstar{\n\tname{ "<< compiledStarName
	 <<"}\n\tdomain{SDF}\n\tdesc{\n"
	 << headerComment() <<"\t}\n\tlocation{ "<<(const char*)destDirectory 
	 << "}\nhinclude {\"Error.h\",\"SimControl.h\"}\n" 
	 << starPorts << "\n\theader{\nextern \"C\" {\n" << include 
	 << "}\n" << globalDecls << procedures << "\t}\nprivate{\n"
	 << mainDecls << "\t}\n\tbegin{\n" << commInit << mainInit 
	 << "\t}\n\tsetup{\n" << starSetup
	 << "\t}\n\tgo{\n" << mainLoop
	 << "\t}\n\twrapup{\n" << mainClose << "\t}\n}";
    myCode.initialize();
    myCode << code;
    initCodeStrings();
}

void CreateSDFStar::writeCode() {
    writeFile(myCode, ".pl", displayFlag);
    CGCTarget::processDependentFiles();

    // Construct makefile
    StringList compileArgs = getCompileOptions(TRUE);
    StringList makefile;
    makefile << headerComment("# ") << "# To make the star, do: " 
	     << "make -f " << (const char*) filePrefix << ".mk all\n"
	     << "ROOT = " << getenv("PTOLEMY") << "\n"
	     << "WORM_INCL = " << starIncludeDirs << " "
			       << compileArgs << "\n\n";
    makefile << "all: " << (const char*) filePrefix << ".o\n"
	     << "include $(ROOT)/mk/cgworm.mk\n\n";
    writeFile(makefile, ".mk");
}

void CreateSDFStar::initCodeStrings() {
    CGCTarget::initCodeStrings();
    starPorts.initialize();
}

int CreateSDFStar::run () {
    return TRUE;
}

// Redefine main loop so that we do not iterate.  The iteration will be
// controlled by calling the go method of the new SDF star
void CreateSDFStar::mainLoopCode() {
    defaultStream = &myCode;
    allWormInputCode();
    compileRun((SDFScheduler*) scheduler());
    CodeStream* crStream = defaultStream;
    defaultStream = &myCode;
    allWormOutputCode();
    defaultStream = crStream;
}

Block* CreateSDFStar::makeNew () const {
    LOG_NEW; return new CreateSDFStar(name(),starType(),descriptor());
}

ISA_FUNC(CreateSDFStar,CGCTarget);
static CreateSDFStar targ("CreateSDFStar","CGCStar","Wormhole target for CGC.");
static KnownTarget entry(targ,"CreateSDFStar");

int CreateSDFStar::linkFiles () {
    StringList linkArgs = getLinkOptions(TRUE);
    StringList dir, linkCmd;

    char* expandedDirName = expandPathName((const char*) destDirectory); 
    dir << expandedDirName;
    linkCmd << dir << "/" << (const char*) filePrefix << ".o " 
	    << starLinkOptions << " " << linkArgs << " ";

    const char* argv[2];
    const char* multiLink = "multilink";

    // The link commands must be in the second argv location
    argv[0] = multiLink;
    argv[1] = hashstring(linkCmd);
    fprintf(stderr, "multiLink: %s\n",argv[1]);
    int status = Linker::multiLink(2,(char**) argv); 
    fprintf(stderr, "multiLink completed\n");
    delete [] expandedDirName;
    return status;
}

int CreateSDFStar::connectStar() {
    // Clone the previously compiled star.
    const char* compiledStarName = filePrefix;
    compiledStarName += 3;
    Block *newCompiledStar = KnownBlock::clone(compiledStarName,"SDF");
    if (newCompiledStar == 0) {
	StringList message;
	message << "Can not clone the compiled CGC-in-SDF worm star: " 
		<< topLevelGalaxy->name();
	Error::abortRun((const char*)message);
	return FALSE;
    }
    // Link the new star into the outer galaxy.
    // Give it the same name the wormhole had.
    Block* topWorm = topLevelGalaxy->parent();
    Galaxy* parentGal = (Galaxy*) topWorm->parent();
    parentGal->addBlock(*newCompiledStar, topWorm->name());

    // Reconnect the connections of the old wormhole star.
    PortHole* wormPort;
    BlockPortIter wpNext(*topWorm);
    while ((wormPort = wpNext++) != 0 ) {
	PortHole* farPort = wormPort->far();
	int delays = wormPort->numInitDelays();
	PortHole* newPort =
	    newCompiledStar->portWithName(ptSanitize(wormPort->name()));
	farPort->disconnect();
	newPort->connect(*farPort,delays);
    }
    
    // We must initialize this new star, the star is created during
    // the initialize method of the parent galaxy, thus the initialize
    // method is run on the original wormhole, and not this
    // dynamically linked in star.
    newCompiledStar->initialize();

    // Tell the parent galaxy to delete the original wormhole.
    parentGal->deleteBlockAfterInit(*topWorm);

    return TRUE;
}
