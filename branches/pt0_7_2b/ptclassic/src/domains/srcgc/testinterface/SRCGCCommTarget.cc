static const char file_id[] = "SRCGCCommTarget.cc";
/******************************************************************
Version identification:
@(#)SRCGCCommTarget.cc  1.40 07/30/96

Copyright (c) 1990-1997 The Regents of the University of California.
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

 Programmer: J. Pino

 Target for Ariel S-56X DSP board.
 
*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCCommTarget.h"
#include "KnownTarget.h"

#include "CGCSRSynchComm.h"
#include "SRCGCCSynchComm.h"
#include "SRCGCCReceive.h"
#include "SRCGCCSend.h"
#include "CGCSRSend.h"
#include "CGCSRReceive.h"
//#include "CGCXPeek.h"
//#include "CGCXPoke.h"
//#include "CG56XCPeek.h"
//#include "CG56XCPoke.h"

SRCGCCommTarget::SRCGCCommTarget(const char* nam, const char* desc,
                                   const char* assocDomain)
                : SRCGCTarget(nam, "SRCGCStar", desc, assocDomain),
                  pairNumber(0) {
  initStates();
}

SRCGCCommTarget::SRCGCCommTarget(const SRCGCCommTarget& arg)
               : SRCGCTarget(arg.name(), arg.starType(),
                             arg.descriptor(), arg.getAssociatedDomain() ),
                 pairNumber(0) {
  initStates();
  copyStates(arg);
}

void SRCGCCommTarget::setup() {
  // Keeps track of how many send/receive communication pairs we create
  SRCGCTarget::setup();
}

void SRCGCCommTarget::initStates() {
}

void SRCGCCommTarget::headerCode () {
}

Block* SRCGCCommTarget::makeNew() const {
  LOG_NEW; return new SRCGCCommTarget(*this);
}

void SRCGCCommTarget::trailerCode () {
  SRCGCTarget::trailerCode();
}

void SRCGCCommTarget::writeCode() {
  SRCGCTarget :: writeCode();
}

int SRCGCCommTarget::runCode() {
  return 1;
}

ISA_FUNC(SRCGCCommTarget,SRCGCTarget);

void SRCGCCommTarget::configureCommPair(CommPair& pair) {
  CGCSRSynchComm* cgcSide = (CGCSRSynchComm*) pair.cgcStar;
  SRCGCCSynchComm* srcgcSide = (SRCGCCSynchComm*) pair.cgStar;
  cgcSide->srcgcSide = srcgcSide;
  srcgcSide->cgcSide = cgcSide;
  //srcgcSide->commCount = cgcSide->commCount = &pairNumber;
  //srcgcSide->pairNumber = cgcSide->pairNumber = pairNumber++;
  
  //cgcSide->S56XFilePrefix.
  // setInitValue(hashstring(filePrefix.currentValue()));
  // cgcSide->S56XFilePrefix.initialize();
}

CommPair SRCGCCommTarget::fromCGC(PortHole&) {
  CommPair pair(new CGCSRSend,new SRCGCCReceive);
  configureCommPair(pair);
  return pair;
}

CommPair SRCGCCommTarget::toCGC(PortHole&) {
  CommPair pair(new CGCSRReceive,new SRCGCCSend);
  configureCommPair(pair);
  return pair;
}


// make an instance
static SRCGCCommTarget proto("SRCGCComm","Communication Target for SRCGC");

static KnownTarget entry(proto, "SRCGCComm");
