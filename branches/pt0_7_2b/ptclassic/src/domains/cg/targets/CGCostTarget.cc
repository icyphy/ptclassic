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

 Programmer:  Raza Ahmed
 Date of creation: 07/09/96

 Target for the generation of cost of individual stars in the galaxy.

***********************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include <string.h>
#include "StringList.h"
#include "CGUtilities.h"
#include "CGCostTarget.h"
#include "Scheduler.h"
#include "miscFuncs.h"
#include "SDFPTclTarget.h"
#include "GalIter.h"
#include "Star.h"
#include "DFPortHole.h"
#include "KnownTarget.h"
#include "pt_fstream.h"
#include "ProfileTimer.h"
#include "KnownBlock.h"

// constructor
CGCostTarget::CGCostTarget(const char* nam, const char* startype,
		       const char* desc) : CGTarget(nam, startype, desc), Galaxy()
{
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
  Galaxy* dummyGlobalGalaxy = new Galaxy;
  dummyGlobalGalaxy = galaxy(); 
  GalAllBlockIter nextBlock(*dummyGlobalGalaxy);
  Block* starBlock = new Block; 
  while((starBlock = (Block*)nextBlock++) != 0) { 
  // If the block is a star, then iterate through the portholes 
         if(starBlock->isItAtomic()) {       

            // Declaring another dummy galaxy which consists of the stars form 
            // the dummyGlobalGalaxy which are then padded up
            Galaxy* dummyGalaxy = new Galaxy;
            dummyGalaxy->addBlock(*starBlock, "starBlock"); 

            // Following code handles any multiports
            MultiPortHole* starBlockMport = new MultiPortHole;           
            BlockMPHIter nextstarBlockMport(*starBlock);
            while((starBlockMport = nextstarBlockMport++) != 0) {
                   if(starBlockMport->isItInput()) {              
                      selectConnectStarBlock(dummyGalaxy, starBlockMport, "input");
		      }
                   else {
                      selectConnectStarBlock(dummyGalaxy, starBlockMport, "output");
                      }
            }
      
  // Following code handles single ports
            PortHole* starBlockport = new PortHole;
            BlockPortIter nextstarBlockport(*starBlock);
            while((starBlockport = nextstarBlockport++) != 0) {
                   if(starBlockport->isItInput()) {
                      selectConnectStarBlock(dummyGalaxy, (MultiPortHole*)starBlockport, "input"); 
                      }
                   else {                                     
                      selectConnectStarBlock(dummyGalaxy, (MultiPortHole*)starBlockport, "output");
                      }
            }

         printGalaxy(dummyGalaxy);
         }
  }
}

void CGCostTarget::selectConnectStarBlock(Galaxy* localGalaxy, MultiPortHole* localHole, char* portName) {
  // The following switch statement will find the right match in 
  // the data type for the block in the galaxy
  // The matched star will be then added to the passed in galaxy 
  // Then the two ports will be connected within the galaxy
  if(strcmp(portName, "input") == 0) {
   
       if(localHole->type() == "INTEGER") {    
                  starPointer = KnownBlock::find("CGDummySourceInt", "CG");
                  localGalaxy->addBlock(*starPointer, "CGDummySourceInt");
                  localHole->connect(*starPointer->portWithName(portName), 0, 0);
          }

       if(localHole->type() == "FIX") {
	          starPointer = KnownBlock::find("CGDummySourceFIX", "CG");
                  localGalaxy->addBlock(*starPointer, "CGDummySourceFIX");
                  localHole->connect(*starPointer->portWithName(portName), 0);
	  }

       if(localHole->type() == "FLOAT") {
	          starPointer = KnownBlock::find("CGDummySourceFLOAT", "CG");
                  localGalaxy->addBlock(*starPointer, "CGDummySourceFLOAT");
                  localHole->connect(*starPointer->portWithName(portName), 0);
	  }
 
       if(localHole->type() == "COMPLEX") {
	          starPointer = KnownBlock::find("CGDummySourceCX", "CG");
                  localGalaxy->addBlock(*starPointer, "CGDummySourceCX");
                  localHole->connect(*starPointer->portWithName(portName), 0);
	  }
		  
     } 
  else {
     starPointer = KnownBlock::find("CGDummySink","CG");
     localGalaxy->addBlock(*starPointer, "CGDummySink");
     localHole->connect(*starPointer->portWithName(portName), 0); 
     }

  
}  

void CGCostTarget::printGalaxy(Galaxy* localGalaxy) {
// The following function will print the information about the
// stars found in the galaxy
  StringList ptclCode;
  ptclCode  << "reset\nnewuniverse " << localGalaxy->name();
  GalStarIter nextStar(*localGalaxy);
  Star* star;
  while((star = nextStar++) != 0) {
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

ISA_FUNC(CGCostTarget,CGTarget);

static CGCostTarget port("CGCostTarget","CGStar","A CG wormhole target");
static KnownTarget entry(port,"CGCostTarget");





