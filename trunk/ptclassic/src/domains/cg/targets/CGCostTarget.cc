static const char file_id[] = "CGCostTarget.cc";
/**********************************************************************
Version identification:
$Id$

Copyright (c) %Q% The Regents of the University of California.
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
 Date of creation: 07/09/96

 Target for the generation of cost of individual stars in the galaxy.

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

// Include files from Ptolemy domains
#include "SDFScheduler.h"

#include "CGUtilities.h"
#include "CGCostTarget.h"

// constructor
CGCostTarget::CGCostTarget(const char* nam, const char* startype,
			   const char* desc, const char* assocDomain) :
CGTarget(nam, startype, desc, assocDomain) {
    tempBlockList.initialize();
}

int CGCostTarget::run() {
    // Call halt request to process pending events and check for halt
    // If the user hit the dismiss button in the run control panel,
    // then the universe referenced by galaxy() will return a null pointer
    int haltFlag = SimControl::haltRequested();
    if (!galaxy() || haltFlag) {
	Error::abortRun("CGCostTarget has no galaxy to process");
	return FALSE;
    }

    // The following code goes through all the non-atomic blocks in
    // the galaxy. Any encountered non-atomic blocks are ignored
    Galaxy* dummyGlobalGalaxy = galaxy();
    GalAllBlockIter nextBlock(*dummyGlobalGalaxy);
    const Block* origStarBlock;
    while((origStarBlock = (Block*)nextBlock++) != 0) { 
	// If the block is a star, then iterate through the portholes 
	if (origStarBlock->isItAtomic()) {       

	    // Declaring another dummy galaxy which consists of the stars form 
	    // the dummyGlobalGalaxy which are then padded up
	    Galaxy* dummyGalaxy = new Galaxy;
	    Block* starBlock = origStarBlock->clone();
	    addTempBlock(starBlock);
	    dummyGalaxy->addBlock(*starBlock, "starBlock"); 

	    // Following code handles any multiports
	    MultiPortHole* starBlockMport;
	    BlockMPHIter nextstarBlockMport(*starBlock);
	    while((starBlockMport = nextstarBlockMport++) != 0) {
		if (!selectConnectStarBlock(dummyGalaxy, starBlockMport)) {
		    Error::warn("CGCostTarget cannot create a test universe ",
				"for the star ", origStarBlock->name());
		}
	    }

	    // Following code handles single ports
	    PortHole* starBlockport;
	    BlockPortIter nextstarBlockport(*starBlock);
	    while((starBlockport = nextstarBlockport++) != 0) {
		if (!selectConnectStarBlock(dummyGalaxy,
					    (MultiPortHole*)starBlockport)) {
		    Error::warn("CGCostTarget cannot create a test universe ",
				"for the star ", origStarBlock->name());
		}
	    }

	    printGalaxy(dummyGalaxy);
	    delete dummyGalaxy;
	    deleteTempBlocks();
	 }
    }
    return TRUE;
}

// Find the right source or sink star to match the block port.
// The matched star will be then added to the passed in galaxy
// Then the two ports will be connected within the galaxy
int CGCostTarget::selectConnectStarBlock(Galaxy* localGalaxy,
		MultiPortHole* localHole) {
    // The matched star will be then added to the passed in galaxy 
    // Then the two ports will be connected within the galaxy
    if (localHole->isItInput()) {
	StringList starName = "CGDummySrc";
	const char* abbreviation = dataTypeAbbreviation(localHole->type());
	if (abbreviation == 0) return FALSE;
	starName << abbreviation;
	const Block* sourceStarPtr = KnownBlock::find(starName, "CG");
	if (sourceStarPtr == 0) return FALSE;
	Block* starPointer = sourceStarPtr->clone();
	addTempBlock(starPointer);
	localGalaxy->addBlock(*starPointer, starName);
	localHole->connect(*starPointer->portWithName("input"), 0, 0);
    } 
    else {
	const Block* sinkStarPtr = KnownBlock::find("CGBlackHole", "CG");
	if (sinkStarPtr == 0) return FALSE;
	Block* starPointer = sinkStarPtr->clone();
	addTempBlock(starPointer);
	localGalaxy->addBlock(*starPointer, "CGBlackHole");
	localHole->connect(*starPointer->portWithName("output"), 0); 
    }
    return TRUE;
}  

void CGCostTarget::printGalaxy(Galaxy* localGalaxy) {
    // The following function will print the information about the
    // stars found in the galaxy
    StringList ptclCode;
    ptclCode << "reset\nnewuniverse " << localGalaxy->name();
    GalStarIter nextStar(*localGalaxy);
    Star* star;
    while ((star = nextStar++) != 0) {
	StringList starName;
	starName << "\"" <<star->fullName() << "\" ";
	StringList setState;
	setState << "\tsetState" << starName;
	ptclCode << "\n\tstar" << starName << "CGCMultiInOut\n"
		 << setState << "name " << star->className() << "\n";
    }
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
