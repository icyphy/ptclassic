static const char file_id[] = "CGCostTarget.cc";
/**********************************************************************
Copyright (c) 1996-%Q% The Regents of the University of California.
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

Programmer:  Raza Ahmed and Brian L. Evans
Created:     07/09/96
Version:     $Id$

Target to generate the cost to implement individual stars in the
galaxy.  The run method constructs a standalone universe for each
star in the galaxy, runs the galaxy, and reports cost information.
For the CG56SimTarget, data memory, program memory, and execution
time estimates will be reported.

***********************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <string.h>

// Include files from the Ptolemy kernel
#include "dataType.h"
#include "StringList.h"
#include "miscFuncs.h"
#include "Scheduler.h"
#include "Star.h"
#include "DFPortHole.h"
#include "KnownTarget.h"
#include "pt_fstream.h"
#include "ProfileTimer.h"
#include "KnownBlock.h"

// Include files from ptcl library
#include "ptclDescription.h"

// Include files from Ptolemy domains
#include "SDFScheduler.h"

#include "CGUtilities.h"
#include "CGCostTarget.h"

// constructor
CGCostTarget::CGCostTarget(const char* nam, const char* startype,
			   const char* desc, const char* assocDomain) :
CGTarget(nam, startype, desc, assocDomain) {
    // Override the default value of the destination directory
    // to be $HOME/PTOLEMY_SYSTEMS/CG/CostTarget
    destDirName = destDirectoryName("CostTarget");
    destDirectory.setInitValue(destDirName);

    // Add a state for the user to list the targets to use to generate
    // implementation costs
    addState(userTargetList.setState("targets", this, "sim-CG56",
	"List of targets to use to generate implementation cost information"));

    // Initalization of other data members
    tempBlockList.initialize();
}

CGCostTarget::~CGCostTarget() {
}

Block* CGCostTarget::makeNew() const {
    LOG_NEW; return new CGCostTarget(name(), starType(), descriptor(),
				     getAssociatedDomain());
}

// For each target specified by the user, generate the implementation costs
// for each star in the galaxy.
int CGCostTarget::run() {
    // Call halt request to process pending events and check for halt
    // If the user hit the dismiss button in the run control panel,
    // then the universe referenced by galaxy() will return a null pointer
    int haltFlag = SimControl::haltRequested();
    if (!galaxy() || haltFlag) {
	Error::abortRun("CGCostTarget has no galaxy to process");
	return FALSE;
    }

    // Generate the Ptcl code for the current galaxy
    if (!convertGalaxyToPtcl(galaxy())) {
	Error::abortRun("CGCostTarget could not find the SDF-to-PTcl target.");
	return FALSE;
    }

    // Each star in the galaxy will be extracted to form a test universe,
    // which will be used to generate implementation costs for that star
    initializeStarPorts(*galaxy());
    disconnectAllStars(*galaxy());

    // Have each target (that the user specified) generate cost information
    int numUserTargets = userTargetList.size();
    for (int i = 0; i < numUserTargets; i++) {
	const char* userTargetName = userTargetList[i];
	CGTarget* userTarget = findCodeGenTarget(userTargetName);
	if (!userTarget) {
	    Error::warn("The target '", userTargetName,
			 "' is not a valid code generation target");
	}
	else if (userTarget->canComputeMemoryUsage() ||
		 userTarget->canComputeExecutionTime() ) {
	    // In order to get the domain associated with the userTarget,
	    // we need to clear the galaxy associated with the userTarget
	    // so that the domain method returns getAssociatedDomain
	    userTarget->clearGalaxy();
	    costInfoForOneTarget(userTarget, userTarget->domain());
	}
	else {
	    Error::warn(userTargetName, " is a valid code generation target ",
		"but cannot generate implementation cost information");
	}
    }

    return TRUE;
}

// Initialize the ports in the parent galaxy
void CGCostTarget::initializeStarPorts(Galaxy& parent) {
    GalStarIter initparentnextStar(parent);
    Star* initparentstar;
    while ((initparentstar = initparentnextStar++) != 0) {
	BlockPortIter initparentnextPort(*initparentstar);
	PortHole* initparentport;
	while((initparentport = initparentnextPort++) != 0){
	    initparentport->initialize();
	    initparentport->resolvedType();
	}
    }
}

// Iterate over all the blocks in the galaxy to disconnect the ports of the
// stars if the connections exist. Otherwise just connect the appropriate
// stars with the stars in the child galaxy and make child galaxies
void CGCostTarget::disconnectAllStars(Galaxy& parent) {
    GalStarIter parentnextStar(parent);
    Star* parentstar;
    while ((parentstar = parentnextStar++) != 0) {
	BlockPortIter parentnextPort(*parentstar);
	PortHole* parentport;
	while((parentport = parentnextPort++) != 0) {
	    PortHole* parentfarPort = parentport->far();
	    // Disconnect connected ports
	    if (parentfarPort) {
		parentport->disconnect();
	    }
	}
    }
}

// Generate implementation cost information for each star in the galaxy.
// Iterate over all blocks in the galaxy and ignore non-atomic blocks.
// When the method exits, the userTarget will have no galaxy attached it.
// FIXME: Should write the cost information to a file
int CGCostTarget::costInfoForOneTarget(
		CGTarget* userTarget, const char* domain) {
    // For each star in the original galaxy, create a child galaxy which
    // consists of the star plus dummy inputs and outputs.  The child
    // is run to generate implementation cost information
    GalStarIter nextStar(*galaxy());
    Star* star;
    userTarget->clearGalaxy();		// disassociate any attached galaxies
    while ((star = nextStar++) != 0) { 
	// Locate the star in the new domain
	const char* starName = star->name();
	const Block* retargettedStar = KnownBlock::find(starName, domain);
	if (retargettedStar == 0) {
	    StringList msg = "Cannot retarget star ";
	    msg << starName << " to the " << domain << " domain";
	    Error::warn(msg);
	    continue;
	}

	// Keep track of errors
	int errorFlag = FALSE;

	// Create a child galaxy
	Galaxy* childGalaxy = new Galaxy;
	childGalaxy->setName("Child");
	childGalaxy->setDomain(domain);

	// Make a copy of the current star
	Block* localChildBlock = retargettedStar->clone();
	addTempBlock(localChildBlock);
	Star& localChildStar = localChildBlock->asStar();
	childGalaxy->addBlock(localChildStar, starName); 

        // Connect a dummy star to each input and output port of the star
	BlockPortIter nextPort(localChildStar);
	PortHole* port;
	while ((port = nextPort++) != 0) {
	    if (!selectConnectStarBlock(childGalaxy, port)) {
		StringList msg = "CGCostTarget cannot build a standalone ";
		msg << "universe for the star" << starName
		    << " in the domain " << domain;
		Error::warn(msg);
		errorFlag = TRUE;
		continue;
	    }
	}

	// Initialize and run the galaxy, and report implementation costs
	if (!errorFlag) {
	    initializeStarPorts(*childGalaxy);
	    userTarget->setState("show memory usage?", "YES");
	    userTarget->setState("show run time?", "YES");
	    userTarget->setGalaxy(*childGalaxy);
	    userTarget->initialize();
	    userTarget->begin();
	    userTarget->run();
	    userTarget->wrapup();
	    reportCosts(userTarget);
	    userTarget->clearGalaxy();
	}

	delete childGalaxy;
	deleteTempBlocks();
    }

    return TRUE;
}

// Find the right source or sink star to match the block port.
// The matched star will be then added to the passed in galaxy
// Then the two ports will be connected within the galaxy
int CGCostTarget::selectConnectStarBlock(
		Galaxy* localGalaxy, PortHole* localHole) {
    // The matched star will be then added to the passed in galaxy 
    // Then the two ports will be connected within the galaxy
    if (localHole->isItInput()) {
	// Find the star CGDummySrcXX where XX is the data type abbrevation
	const char* abbreviation = dataTypeAbbreviation(localHole->type());
	if (abbreviation == 0) return FALSE;
	StringList starName = "DummySrc";
	starName << abbreviation;
	const Block* sourceStarPtr = KnownBlock::find(starName, "CG");
	if (sourceStarPtr == 0) return FALSE;

	// Add dummy source star to galaxy and connect it to porthole localHole
	Block* starPointer = sourceStarPtr->clone();
	addTempBlock(starPointer);
	localGalaxy->addBlock(*starPointer, starName);
	localHole->connect(*starPointer->portWithName("input"), 0, "");
    } 
    else {
	// Find the CGBlackHole star
	const char* starName = "BlackHole";
	const Block* sinkStarPtr = KnownBlock::find(starName, "CG");
	if (sinkStarPtr == 0) return FALSE;

	// Add black hole to galaxy and connect it to porthole localHole
	Block* starPointer = sinkStarPtr->clone();
	addTempBlock(starPointer);
	localGalaxy->addBlock(*starPointer, starName);
	localHole->connect(*starPointer->portWithName("output"), 0, ""); 
    }
    return TRUE;
}  

// Find the code generation target corresponding to a target name
CGTarget* CGCostTarget::findCodeGenTarget(const char* userTargetName) {
    // Check to make sure that the target exists and is a CG target
    const Target* userTarget = KnownTarget::find(userTargetName);
    if (userTarget && userTarget->isA("CGTarget")) {
	return (CGTarget*) userTarget;
    }
    return (CGTarget*) 0;
}

// Write out the equivalent Ptcl code for the galaxy to the file
// destDirectory/galaxyName.pt, and also write out a PTcl header
int CGCostTarget::convertGalaxyToPtcl(Galaxy* localGalaxy) {
    StringList ptclFileName;
    ptclFileName << destDirectory << "/" << localGalaxy->name() << ".pt";
    ptclDescription(localGalaxy, ptclFileName, TRUE);
    return TRUE;
}

// Reports the implementation costs of the galaxy
// FIXME: The implemenation costs should be written to a file
void CGCostTarget::reportCosts(CGTarget* userTarget) {
    // The following function will print the information about the
    // stars found in the galaxy
    userTarget->computeImplementationCost();
    cout << userTarget->describeImplementationCost() << "\n";
}

void CGCostTarget::wrapup() {
}

void CGCostTarget::deleteTempBlocks() {
    ListIter bdel(tempBlockList);
    Block *bd;
    while ( (bd = (Block *)bdel++) ) {
	  LOG_DEL; delete bd;
    }
    tempBlockList.initialize();
}

const char* CGCostTarget::dataTypeAbbreviation(const char* datatype) {
    const char* abbreviation = 0;
    switch(datatype[0]) {
      case 'C':
	if (strcmp(datatype, "COMPLEX") == 0) abbreviation = "Cx";
	break;

      case 'F':
	if (strcmp(datatype, "FLOAT") == 0) abbreviation = "";
	else if (strcmp(datatype, "FIX") == 0) abbreviation = "Fix";

      case 'I':
	if (strcmp(datatype, "INTEGER") == 0) abbreviation = "Int";
    }
    return abbreviation;
}

ISA_FUNC(CGCostTarget, CGTarget);

static CGCostTarget port("CGCostTarget", "CGStar", "A CG wormhole target");
static KnownTarget entry(port,"CGCostTarget");
