static const char file_id[] = "CompileCGSubsystems.cc";

/*****************************************************************
Version identification:
@(#)CompileCGSubsystems.cc	1.16 01/15/97

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

Programmer: Jose Luis Pino

*****************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "CompileCGSubsystems.h"
#include "GalIter.h"
#include "CGCFork.h"
#include "Wormhole.h"
#include "KnownTarget.h"
#include "AnyCGStar.h"
#include "ParScheduler.h"

#include "CreateSDFStar.h"
#include "CGCSDFReceive.h"
#include "CGCSDFSend.h"
#include "DynamicGalaxy.h"

class DummyComm : public AnyCGStar {
public:
    DummyComm(int t, int f):AnyCGStar(),partner(0),to(t),from(f){};
    DummyComm():AnyCGStar(),partner(0),to(0),from(0){};
    DummyComm *partner;
    int to, from;
};

class DummySend : public DummyComm {
public:
    InAnyCGPort input;
    DummySend(int t, int f):DummyComm(t,f) {
	addPort(input.setPort("input",this,ANYTYPE));
    }
};

class DummyReceive : public DummyComm {
public:
    OutAnyCGPort output;
    DummyReceive(int t, int f):DummyComm(t,f) {
	addPort(output.setPort("output",this,ANYTYPE));
    }
};

CompileCGSubsystems::
CompileCGSubsystems(const char* name,const char* starType,const char* desc):
CGSharedBus(name,starType,desc),cgcWorm(0) {
    childType.setInitValue("default-CGC");
    destDirName = destDirectoryName("MIXED");
    destDirectory.setInitValue(destDirName);
}

void CompileCGSubsystems::prepareChildren() {
    CGSharedBus::prepareChildren();
    if(!galaxy()->parent() || !galaxy()->parent()->isItWormhole()) return;
    if(!child(0)->isA("CreateSDFStar")) {
	Error::abortRun(*child(0)," is not a CreateSDFStar.");
	return;
    }
}

void CompileCGSubsystems::wormPrepare() {
    if(!galaxy()->parent() || !galaxy()->parent()->isItWormhole()) return;
    cgcWorm = (CreateSDFStar*)child(0);
    cgcWorm->convertWormholePorts(*galaxy());
}

void CompileCGSubsystems::flattenWorm() {
  // Call the parent method.
  CGSharedBus::flattenWorm();

  // NOTE: This code must come after flattening wormholes, or else you
  // won't be able to iterate through all the stars in the universe,
  // because wormholes act as stars and hide the stars inside them from
  // the GalStarIter iterator.

  // NOTE: This code is a partial hack to insert CGCFork stars in between
  // CGStars that have been manually assigned different procIds because
  // current multiprocessor scheduling can't handle splicing communication
  // links other than simple 2-star connections.  We put in the CGCStar
  // to force the scheduler to go through CGC because it doesn't know how
  // to go between two non-CGC domains.
  // This code doesn't check to see if two connected CGCStars have
  // different procIds.

  // Insert CGCFork stars in between stars with differing procIds.
  GalStarIter nextStar(*galaxy());
  CGStar* star;
  while  ((star = (CGStar*) nextStar++) != NULL) {
    BlockPortIter nextPort(*star);
    PortHole* port;
    while ((port = nextPort++) != NULL) {
      PortHole* farPort = port->far();
      if (!farPort) {
	Error::abortRun(*port, "is not connected");
	return;
      }
      else {
	CGStar* farStar = (CGStar*) farPort->parent();
	if (!farStar) {
	  Error::abortRun(*farPort, "has no parent");
	  return;
	}
	else {
	  if (!star->isA("CGCStar")) {
	    if (!farStar->isA("CGCStar")) {
	      if ((star->getProcId() >= 0)) {
		if ((farStar->getProcId() >=0)) {
		  if (star->getProcId() != farStar->getProcId()) {
		    int numDelays = port->numInitDelays();
		    const char* delayValues = port->initDelayValues();
		    port->disconnect();
		    CGCFork* cgcFork = new CGCFork;
		    // FIXME: I assume procId should be 0 for CGC.
		    cgcFork->setProcId(0);
		    galaxy()->addBlock(*cgcFork,"CGCWormFork");
		    PortHole& cgcForkOutput = cgcFork->output.newConnection();
		    if (farPort->isItOutput()) {
		      farPort->connect(cgcFork->input,numDelays, delayValues);
		      cgcForkOutput.connect(*port,0,"");
		    }
		    else {
		      port->connect(cgcFork->input,numDelays, delayValues);
		      cgcForkOutput.connect(*farPort,0,"");
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

int CompileCGSubsystems::modifyGalaxy() {
  // Let HOF type stars do their magic
  if (!CGSharedBus::modifyGalaxy()) return FALSE;
    
  GalStarIter nextStar(*galaxy());
  CGStar* star;
  // FIXME - This code only processes top-level wormholes
  while ((star = (CGStar*)nextStar++) != NULL) {
    Wormhole* worm = star->asWormhole();
    if (!worm) continue;
    BlockPortIter nextPort(*star);
    PortHole* port;
    while ((port = nextPort++) != NULL) {
      if (port->atBoundary()) continue;
      Wormhole* farWorm = ((Star*)port->far()->parent())->asWormhole();
      const char* farDomain = farWorm?
	farWorm->insideDomain():port->far()->parent()->domain();
      const char* wormDomain = worm->insideDomain();
      if (strcmp(farDomain,wormDomain) != 0 &&
	  strcmp(farDomain,"CGC") != 0 && 
	  strcmp(wormDomain,"CGC") != 0) {
	PortHole* farPort = port->far();
	int numDelays = port->numInitDelays();
	const char* delayValues = port->initDelayValues();
	port->disconnect();
	CGCFork* cgcFork = new CGCFork;
	galaxy()->addBlock(*cgcFork,"CGCWormFork");
	PortHole& cgcForkOutput = cgcFork->output.newConnection();
	if (farPort->isItOutput()) {
	  farPort->connect(cgcFork->input,numDelays, delayValues);
	  cgcForkOutput.connect(*port,0,"");
	}
	else {
	  port->connect(cgcFork->input,numDelays, delayValues);
	  cgcForkOutput.connect(*farPort,0,"");
	}
      }
    }
  }
  return TRUE;
}

DataFlowStar* CompileCGSubsystems::createSend(int from, int to, int /*num*/) {
    return new DummySend(to,from);
}
    
DataFlowStar* CompileCGSubsystems::createReceive(int from, int to, int /*num*/) {
    return new DummyReceive(to,from);
}

inline DFPortHole* singlePort(DataFlowStar& star) {
    if (star.numberPorts() != 1) {
	StringList message;
	message << "returnSinglePort: The star, "
		<< star.className() << ", does not have "
		"the only 1 port; it has "
		<< star.numberPorts() << "ports.";
	Error::abortRun(message);
	return 0;
    }
    BlockPortIter ports(star);
    return (DFPortHole*)ports++;
}

int CompileCGSubsystems::replaceCommBlock
(DataFlowStar& newStar, DataFlowStar& oldStar) {
    if (!oldStar.parent()) {
	Error::abortRun(oldStar,"replaceCommBlock: The old "
			"communication star has no parent galaxy.");
	return FALSE;
    }
    Galaxy& subGalaxy = oldStar.parent()->asGalaxy();
    subGalaxy.removeBlock(oldStar);
    subGalaxy.addBlock(newStar,newStar.name());
    DFPortHole *newPort, *oldPort;
    if ((newPort = singlePort(newStar)) == NULL) return FALSE;
    if ((oldPort = singlePort(oldStar)) == NULL) return FALSE;
    newStar.repetitions = oldStar.repetitions;

    PortHole *farPort = oldPort->far();
    int numDelays = oldPort->numInitDelays();
    const char* delayValues = oldPort->initDelayValues();
    int numSample = oldPort->numXfer();
    oldPort->disconnect();

    farPort->connect(*newPort,numDelays,delayValues);
    newPort->setSDFParams(numSample,numSample-1);

    if (! ((ParScheduler*)scheduler())->myProcs()
	->replaceCommStar(newStar,oldStar)) {
	Error::abortRun(*this,"Could not replace Send/Receive stars.");
	return FALSE;
    }

    delete &oldStar;
    return TRUE;
}
	
void CompileCGSubsystems::pairSendReceive(DataFlowStar* oldSend,
					  DataFlowStar* oldReceive) {
    int from = ((DummyComm*)oldSend)->from;
    int to = ((DummyComm*)oldSend)->to;

    PortHole* dummyPort = singlePort(*oldSend);
    if (dummyPort == NULL) return;
    
    // FIXME - Assumes CGC is child 0
    CommPair pair;

    DataFlowStar *newSend, *newReceive;
    if (from == 0) {
	pair = cgChild(to)->fromCGC(*dummyPort);
	if (SimControl::haltRequested()) return;
	newSend = pair.cgcStar;
	newReceive = pair.cgStar;
    }
    else if (to == 0) {
	pair = cgChild(from)->toCGC(*dummyPort);
	if (SimControl::haltRequested()) return;
	newSend = pair.cgStar;
	newReceive = pair.cgcStar;
    }
    else {
      if (Error::canMark()) {
	PortHole* oldInput = oldSend->portWithName("input");
	if (oldInput) {
	  PortHole* oldInFar = oldInput->far();
	  if (oldInFar) {
	    Block* fromStar = oldInFar->parent();
	    if (fromStar) {
	      Error::mark(*fromStar);
	    }
	  }
	}
	PortHole* oldOutput = oldReceive->portWithName("output");
	if (oldOutput) {
	  PortHole* oldOutFar = oldOutput->far();
	  if (oldOutFar) {
	    Block* toStar = oldOutFar->parent();
	    if (toStar) {
	      Error::mark(*toStar);
	    }
	  }
	}
      }
      Error::abortRun("Neither side of the comm link is a CGC star.\n",
		      "Currently we can only communicate through CGC.\n",
		      "Try splicing in a CGCFork star between the two sides.");
      return;
    }

    if(!replaceCommBlock(*newSend,*oldSend) ||
       !replaceCommBlock(*newReceive,*oldReceive)) {
	delete newSend;
	delete newReceive;
	return;
    }

    StringList link = symbol("Link");
    StringList send, receive;
    send << "Send_" << link;
    receive << "Receive_" << link;
    newSend->setName(hashstring(send));
    newReceive->setName(hashstring(receive));
    CGSharedBus::pairSendReceive(newSend,newReceive);
}

void CompileCGSubsystems::generateCode() {
    if (haltRequested()) return;
    if (inWormHole()) CGSharedBus::begin();
    CGSharedBus::generateCode();
}

int CompileCGSubsystems::runCode() {	
    // FIXME - assumes cgc target is child 0
    return cgChild(0)->runCode();
}

ISA_FUNC(CompileCGSubsystems,CGSharedBus);

const char* CompileCGSubsystems::className() const {
    return "CompileCGSubsystems";
}

int CompileCGSubsystems::childIsA(const char* /*type*/) const {
    return TRUE;
    // FIXME - this doesn't work - the name given by type is not
    // always the name the target is registered under
    // const Target* targetType = KnownTarget::find(type);
    // return targetType? targetType->isA("CGTarget"):FALSE;
}

static CompileCGSubsystems cgCompileCGSubsystems("CompileCGSubsystems","CGStar","A CG wormhole target");
static KnownTarget entry(cgCompileCGSubsystems,"CompileCGSubsystems");
