static const char file_id[] = "ArchTarget.cc";
/******************************************************************
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

 Programmer: Michael C. Williamson

 Base target for VHDL code generation.

*******************************************************************/

#ifdef __GNUG__
#pragma implementation
#endif

#include "ArchTarget.h"
#include "KnownTarget.h"
#include "HashTable.h"

#include <fstream.h>

// Constructor.
ArchTarget :: ArchTarget(const char* name,const char* starclass,
			 const char* desc) :
SimVSSTarget(name,starclass,desc) {
  systemClockUsed = 0;
  regIntsUsed = 0;
  regRealsUsed = 0;
  muxIntsUsed = 0;
  muxRealsUsed = 0;
  sorIntsUsed = 0;
  sorRealsUsed = 0;

  // Maybe uncomment the following if not simulating:
  // Set the default to display the code.
  //  analyze.setInitValue("NO");

  // Set the default to not use loop scheduling.
  loopingLevel.setInitValue("0");

  addCodeStreams();
  initCodeStreams();
}

// Clone the Target.
Block* ArchTarget :: makeNew() const {
  LOG_NEW; return new ArchTarget(name(), starType(), descriptor());
}

static ArchTarget proto("arch-VHDL", "VHDLStar",
			 "VHDL code generation target with structural style");
static KnownTarget entry(proto,"arch-VHDL");

void ArchTarget :: begin() {
  SimVSSTarget::begin();
}

// Setup the target.
void ArchTarget :: setup() {
  if (galaxy()->isItWormhole()) {
    // Currently this does not seem to get invoked:
    Error::warn("This galaxy is a wormhole!");
  }
  if (galaxy()) setStarIndices(*galaxy()); 
  
  SimVSSTarget::setup();

  clockList.initialize();

  // Need to make this conditional depending on if doing sim or synth.
  setSystemClockUsed();
}

// Main routine.
int ArchTarget :: runIt(VHDLStar* s) {
  // Create a new VHDLFiring to load up.
  VHDLFiring* fi = new VHDLFiring;
  // Set the current firing to point to this firing.
  currentFiring = fi;

  // Begin constructing the components of the firing's code.
  StringList tempName = s->fullName();

  StringList clsName = s->className();
  if (!strcmp(clsName,"VHDLCSend") || !strcmp(clsName,"VHDLCReceive")) {
    fi->noSensitivities = TRUE;
    fi->noOutclocking = TRUE;
    // Do not tack on the firing number to the name.
  }
  else {
    fi->noSensitivities = FALSE;
    fi->noOutclocking = FALSE;
    // Tack on the firing number to the name.
    tempName << "_F" << s->firingNum();
  }
  StringList fiName = sanitize(tempName);

  fi->setName(fiName);

  // Change the default stream temporarily.
  defaultStream = &firingAction;
  // Initialize lists for new firing.
  initFiringLists();
  // Process action; running star modifies myCode.
  int status = ((CGStar*) s)->CGStar::run();
  // Change the default stream back.
  defaultStream = &myCode;

  // Do not create firing data for forks.
  if (s->isItFork()) {
    return status;
  }

  // Search for a firing in the list with the same name as this one.
  // We're trying to find repeated firings of the same send/receive stars
  // so that they can be handled specially.
  int foundFiring = 0;

  VHDLFiring* existFiring;
  if ((existFiring = masterFiringList.vhdlFiringWithName(fiName)) != 0) {
    foundFiring = 1;
    // We found a firing with the same name.
    // Presume it to be a send/receive star, since only those
    // have non-unique names from firing to firing.
    // Add to the generic list.
    existFiring->genericList->addList(firingGenericList);
    // Add to the port list.
    existFiring->portList->addList(firingPortList);
    // Add to the signal list.
    existFiring->signalList->addList(firingSignalList);
    // Add to the declarations.
    existFiring->decls << mainDecls;
    // Add to the local variable declarations.
    existFiring->decls << addVariableDecls(&localVariableList, 0, 1);
    // Add to the variable list.
    existFiring->variableList->addList(firingVariableList);
    // Add to the action list.
    StringList naction = "";
    naction << preSynch;
    naction << firingAction;
    naction << postSynch;
    existFiring->action << naction;
    preSynch.initialize();
    firingAction.initialize();
    postSynch.initialize();
  }

  if (!foundFiring) {
    fi->starClassName = s->className();
    fi->genericList = firingGenericList.newCopy();
    fi->portList = firingPortList.newCopy();
    fi->signalList = firingSignalList.newCopy();
    fi->decls = mainDecls;
    mainDecls.initialize();

    // If there are local/temporary variables, put them into
    // mainDecls here.  Then clear the localVariableList.
    fi->decls << addVariableDecls(&localVariableList, 0, 1);
    localVariableList.initialize();

    fi->variableList = firingVariableList.newCopy();

    StringList action = "";
    action << preSynch;
    action << firingAction;
    action << postSynch;
  
    fi->action = action;

    preSynch.initialize();
    firingAction.initialize();
    postSynch.initialize();

    // Put the firing in the firingList.
    masterFiringList.put(*fi);
  }

  // Vestigial code - see original for reasoning behind this, then change it.
  if (!status) {
    return status;
  }
  return status;
}

// Redefine generateCode() here in order to do iterative code gen and synthesis.
void ArchTarget::generateCode() {
	if (haltRequested()) return;
	if (parent()) setup();
	if (haltRequested()) return;
	if(!allocateMemory()) {
		Error::abortRun(*this,"Memory allocation error");
		return;
	}
	if (haltRequested()) return;
	generateCodeStreams();
	if (haltRequested()) return;
	frameCode();
	if (haltRequested()) return;
	if (!parent()) writeCode();

	// Call wrapup to compile, load and run the code.
	wrapup();
	frameCode();
	writeCode();

	// now initialize for the next run
	procedures.initialize();
}

// Initial stage of code generation (done first).
void ArchTarget :: headerCode() {
}

// Trailer code (done last).
void ArchTarget :: trailerCode() {
  // Iterate through all firings and all ports.
  // Create input signals and muxes for input ports.
  // Create output signals for output ports.
  {
    VHDLFiringListIter nextFiring(masterFiringList);
    VHDLFiring* firing;
    while ((firing = nextFiring++) != 0) {
      VHDLPortListIter nextPort(*(firing->portList));
      VHDLPort* port;
      while ((port = nextPort++) != 0) {
	if (port->isItInput()) {
	  VHDLSignal* newSignal = new VHDLSignal;

	  StringList newSignalName = port->getName();
	  newSignal->setName(newSignalName);
	  newSignal->setType(port->getType());

	  signalList.put(*newSignal);

	  port->connect(newSignal);

	  VHDLMux* newMux = new VHDLMux;
	  StringList muxName = firing->getName();
	  muxName << "_" << port->getName();

	  newMux->setName(muxName);
	  newMux->setType(port->getType());
	  newMux->setOutput(newSignal);

	  // Temporary control singal until it gets re-set.
	  newMux->setControl(newSignal);

	  muxList.put(*newMux);
	}
	if (port->isItOutput()) {
	  VHDLSignal* newSignal = new VHDLSignal;

	  StringList newSignalName = port->getName();
	  newSignal->setName(newSignalName);
	  newSignal->setType(port->getType());

	  signalList.put(*newSignal);

	  port->connect(newSignal);
	}
      }
    }
  }

  // Iterate through tokenList and create registers for each token.
  VHDLTokenListIter nextToken(tokenList);
  VHDLToken* token;
  while ((token = nextToken++) != 0) {
    //    cout << "Token:  " << token->getName() << "\n";
    cout.flush();

    VHDLSignal* newSignal = new VHDLSignal;
    newSignal->setName(token->getName());
    newSignal->setType(token->getType());

    signalList.put(*newSignal);

    VHDLReg* newReg = new VHDLReg;
    newReg->setName(token->getName());
    newReg->setType(token->getType());
    newReg->setOutput(newSignal);

    VHDLSignal* clockSignal =
      signalList.vhdlSignalWithName(token->clockName);
    if (clockSignal) {
      newReg->setClock(clockSignal);
    }
    else {
      // No clock signal set for token - must be generated
      // from feedback from previous iterations.
      StringList clockName = "feedback_clock";
      StringList clockType = "BOOLEAN";

      clockSignal = new VHDLSignal;
      clockSignal->setName(clockName);
      clockSignal->setType(clockType);
      signalList.put(*clockSignal);

      newReg->setClock(clockSignal);
    }

    // Connect the reg of the token to
    // the signal of the port of the source firing of the token.
    VHDLFiring* sourceFiring;
    sourceFiring = token->getSourceFiring();
    if (sourceFiring) {
      // Now get the real firing out of the firing list.
      sourceFiring =
	masterFiringList.vhdlFiringWithName(sourceFiring->getName());
      // Get the real port from the firing's port list.
      StringList tokenOutPortName = token->getName();
      tokenOutPortName << "_OUT";
      VHDLPort* realPort =
	(sourceFiring->portList)->vhdlPortWithName(tokenOutPortName);
      if (realPort) {
	/*
	cout << "  sourceFiring:  " << "firing:  " << sourceFiring->getName();
	cout << "  port:  ";
	cout << realPort->getName();
	cout << "\n";
	cout.flush();
	*/
	newReg->setInput(realPort->signal);
      }
      else {
	Error::error(sourceFiring->getName(),
		     ": has no port with name ", tokenOutPortName);
	return;
      }
    }
    else {
      /*
      cout << "Token:  " << token->getName() << ": has no sourceFiring\n";
      */
    }
    // Finally, put the reg in the list AFTER its members have been set.
    regList.put(*newReg);

    // Connect the signal of the token to
    // the mux of the port of each dest firing of the token.
    VHDLFiringList* destFiringList = token->getDestFirings();
    VHDLFiringListIter nextFiring(*destFiringList);
    VHDLFiring* destFiring;
    if (destFiringList->size()) {
      while ((destFiring = nextFiring++) != 0) {
	// Now get the real firing out of the firing list.
	destFiring =
	  masterFiringList.vhdlFiringWithName(destFiring->getName());
	// Get the real port from the firing's port list.
	StringList tokenInPortName = token->getName();
	tokenInPortName << "_IN";
	VHDLPort* realPort =
	  (destFiring->portList)->vhdlPortWithName(tokenInPortName);
	if (realPort) {
	  /*
	  cout << "  destFiring:  " << "firing:  " << destFiring->getName();
	  cout << "  port:  ";
	  cout << realPort->getName();
	  cout << "\n";
	  cout.flush();
	  */
	  StringList muxName = destFiring->getName();
	  muxName << "_" << realPort->getName();
	  VHDLMux* existMux = muxList.vhdlMuxWithName(muxName);
	  if (existMux) {
	    newSignal->setControlValue(1);
	    existMux->addInput(newSignal);
	  }
	  else {
	    Error::error(muxName,
			 ": no such mux in muxList");
	    return;
	  }
	}
	else {
	  Error::error(destFiring->getName(),
		       ": has no port with name ", tokenInPortName);
	  return;
	}
      }
    }
    else {
      /*
      cout << "Token:  " << token->getName() << ": has no destFirings\n";
      */
    }
  }

  // Iterate through the state list and connect registers and
  // initial value multiplexors for each referenced state.
  VHDLStateListIter nextState(stateList);
  VHDLState* state;
  while ((state = nextState++) != 0) {
    // If its a constant source, need to do things a bit differently:
    // only have a source with a signal, feedback of state.
    if (state->constant) {
      StringList initName = state->name;
      initName << "_INIT";
      signalList.put(initName, state->type);
      VHDLSignal* initSignal = signalList.vhdlSignalWithName(initName);
      if (initSignal) {
	connectSource(state->initVal, initSignal);
      }
      else {
	Error::abortRun(initName,
			": no such signal with this name in signalList");
	return;
      }

      // FIXME:  dummy signal for control just to please mux maker.
      // if only one input signal, mux should get replaced anyway
      VHDLSignal* firstIterDone =
	signalList.vhdlSignalWithName("FIRST_ITER_DONE");
      if (!firstIterDone) {
	firstIterDone = new VHDLSignal;
	firstIterDone->setName("FIRST_ITER_DONE");
	firstIterDone->setType("INTEGER");
	signalList.put(*firstIterDone);
      }

      // Need to connect the signal from the constant state's source
      // to all firings that input the constant state, and not just
      // the first one.
      StringListIter nextName(state->constRefFiringList);
      const char* refFiringName;
      while ((refFiringName = nextName++) != 0) {
	StringList stateRefPortName = state->firstRef;
	stateRefPortName << "_IN";
	StringList muxName = refFiringName;
	muxName << "_" << stateRefPortName;
	VHDLMux* stateRefMux =
	  muxList.vhdlMuxWithName(muxName);
	if (!stateRefMux) {
	  Error::abortRun(muxName, ": no such mux in muxList");
	  return;
	}
	else {
	  stateRefMux->addInput(initSignal);
	}

	stateRefMux->setControl(firstIterDone);
      }

      initSignal->setControlValue(0);
      signalList.put(*initSignal);

      /*
      StringList initName = state->name;
      initName << "_INIT";
      signalList.put(initName, state->type);
      VHDLSignal* initSignal = signalList.vhdlSignalWithName(initName);
      if (initSignal) {
	connectSource(state->initVal, initSignal);
      }
      else {
	Error::abortRun(initName,
			": no such signal with this name in signalList");
	return;
      }

      VHDLReg* firstRefReg =
	regList.vhdlRegWithName(state->firstRef);
      if (!firstRefReg) {
	Error::abortRun(state->firstRef, ": no such reg in regList");
	return;
      }
      else {
	firstRefReg->setInput(initSignal);
	VHDLSignal* startClockSignal =
	  signalList.vhdlSignalWithName("start_clock");
	if (!startClockSignal) {
	  startClockSignal = new VHDLSignal;
	  startClockSignal->setName("start_clock");
	  startClockSignal->setType("BOOLEAN");
	  signalList.put(*startClockSignal);
	}
	firstRefReg->setClock(startClockSignal);
      }
      */
    }
    // If firings change state, need to connect a reg and a mux
    // between the lastRef to the state and the firstRef to the state.
    if (!(state->constant)) {
      StringList initName = state->name;
      initName << "_INIT";

      VHDLSignal* initSignal = new VHDLSignal;
      initSignal->setName(initName);
      initSignal->setType(state->type);

      VHDLSignal* lastRefSignal =
	signalList.vhdlSignalWithName(state->lastRef);
      VHDLReg* firstRefReg =
	regList.vhdlRegWithName(state->firstRef);

      StringList firstRefPortName = state->firstRef;
      firstRefPortName << "_IN";
      StringList muxName = state->firstFiringName;
      muxName << "_" << firstRefPortName;
      VHDLMux* firstRefMux =
	muxList.vhdlMuxWithName(muxName);
      if (!lastRefSignal) {
	Error::abortRun(state->lastRef, ": no such signal in signalList");
	return;
      }
      else if (!firstRefReg) {
	Error::abortRun(state->firstRef, ": no such reg in regList");
	return;
      }
      else if (!firstRefMux) {
	Error::abortRun(muxName, ": no such mux in muxList");
	return;
      }
      else {
	firstRefReg->setInput(lastRefSignal);
	firstRefMux->addInput(initSignal);
	connectSource(state->initVal, initSignal);
      }

      /*
      // NEW: Add a dependency over iterations between
      // the last ref signal and the first ref reg.
      VHDLDependency* dep = new VHDLDependency;
      dep->source = lastRefSignal;
      dep->sink = firstRefReg;
      // Need dep to have unique name before putting into list.
      StringList depName = "";
      depName << dep->source->getName() << "," << dep->sink->getName();
      dep->setName(depName);
      iterDependencyList.put(*dep);
      // END NEW
      */

      // NEW: Add a dependency over iterations between
      // the last ref firing and the first ref reg.
      VHDLDependency* dep = new VHDLDependency;
      StringList lastFiringName = state->lastFiringName;
      VHDLFiring* lastFiring =
	masterFiringList.vhdlFiringWithName(lastFiringName);
      if (lastFiring) {
	dep->source = lastFiring;
      }
      else {
	Error::error(lastFiringName,
		     ":  Couldn't find a firing with this name");
      }
      dep->sink = firstRefReg;
      // Need dep to have unique name before putting into list.
      StringList depName = "";
      depName << dep->source->getName() << "," << dep->sink->getName();
      dep->setName(depName);
      dependencyList.put(*dep);
      // END NEW

      VHDLSignal* iterClockSignal =
	signalList.vhdlSignalWithName("start_clock");
      if (!iterClockSignal) {
	iterClockSignal = new VHDLSignal;
	iterClockSignal->setName("start_clock");
	iterClockSignal->setType("BOOLEAN");
	signalList.put(*iterClockSignal);
      }
      firstRefReg->setClock(iterClockSignal);

      VHDLSignal* firstIterDone =
	signalList.vhdlSignalWithName("FIRST_ITER_DONE");
      if (!firstIterDone) {
	firstIterDone = new VHDLSignal;
	firstIterDone->setName("FIRST_ITER_DONE");
	firstIterDone->setType("INTEGER");
	signalList.put(*firstIterDone);
      }

      firstRefMux->setControl(firstIterDone);
      initSignal->setControlValue(0);
      signalList.put(*initSignal);
    }
  }

  // Temporary, because it's used elsewhere.
  {
    VHDLSignal* iterClockSignal =
      signalList.vhdlSignalWithName("start_clock");
    if (!iterClockSignal) {
      iterClockSignal = new VHDLSignal;
      iterClockSignal->setName("start_clock");
      iterClockSignal->setType("BOOLEAN");
      signalList.put(*iterClockSignal);
    }

    VHDLSignal* firstIterDone =
      signalList.vhdlSignalWithName("FIRST_ITER_DONE");
    if (!firstIterDone) {
      firstIterDone = new VHDLSignal;
      firstIterDone->setName("FIRST_ITER_DONE");
      firstIterDone->setType("INTEGER");
      signalList.put(*firstIterDone);
    }
  }

  // Iterate through the arc list.
  // Track the read/write references made on each arc.
  VHDLArcListIter nextArc(arcList);
  VHDLArc* arc;
  while ((arc = nextArc++) != 0) {
    // Determine which signals need to be fed back through registers.
    // All signals which were read, but weren't written, must be latched.
    for (int ix = arc->lowRead; ix < arc->lowWrite; ix++) {
      StringList sourceName = arc->name;
      StringList destName = arc->name;
      int sx = (ix + (arc->highWrite - arc->lowWrite) + 1);
      if (sx >= 0) {
	sourceName << "_" << sx;
      }
      else { /* (sx < 0) */
	sourceName << "_N" << (-sx);
      }
      if (ix >= 0) {
	destName << "_" << ix;
      }
      else { /* (ix < 0) */
	destName << "_N" << (-ix);
      }

      VHDLSignal* sourceSignal = new VHDLSignal;
      sourceSignal->setName(sourceName);
      sourceSignal->setType(arc->getType());
      VHDLSignal* destSignal = new VHDLSignal;
      destSignal->setName(destName);
      destSignal->setType(arc->getType());

      signalList.put(*sourceSignal);
      signalList.put(*destSignal);

      // FIXME: feedback_clock and first_iter_done should be merged.
      StringList clockName = "feedback_clock";
      StringList clockType = "BOOLEAN";

      VHDLSignal* clockSignal =
	signalList.vhdlSignalWithName(clockName);
      if (!clockSignal) {
	clockSignal = new VHDLSignal;
	clockSignal->setName(clockName);
	clockSignal->setType(clockType);
	signalList.put(*clockSignal);
      }

      VHDLReg* sourceReg = regList.vhdlRegWithName(sourceName);
      if (sourceReg) {
	if (sourceReg->getOutput()) {
	  // Reg already created by token and output already set.
	  sourceSignal = sourceReg->getOutput();
	}
	else {
	  sourceReg->setOutput(sourceSignal);
	}
      }
      else {
	// Need to create the reg and set it up appropriately.
	sourceReg = new VHDLReg;
	sourceReg->setName(sourceName);
	sourceReg->setType(arc->getType());
	sourceReg->setOutput(sourceSignal);
	sourceReg->setClock(clockSignal);

	regList.put(*sourceReg);
      }

      VHDLReg* destReg = regList.vhdlRegWithName(destName);
      if (destReg) {
	if (destReg->getInput()) {
	  Error::error(destReg->getName(),
		       "Reg's input set already, but shouldn't be yet.");
	}
	else {
	  // Source signal is the signal out of source register.
	  // Dest signal is the signal out of dest register.
	  destReg->setInput(sourceSignal);
	}
      }
      else {
	// Need to create the reg and set it up appropriately.
	destReg = new VHDLReg;
	destReg->setName(destName);
	destReg->setType(arc->getType());
	destReg->setInput(sourceSignal);
	destReg->setClock(clockSignal);

	regList.put(*destReg);
      }

      VHDLToken* sourceToken =
	tokenList.vhdlTokenWithName(sourceName);
      VHDLToken* destToken =
	tokenList.vhdlTokenWithName(destName);

      // Add a dependency over iterations between
      // the source reg and the dest reg.
      VHDLDependency* dep = new VHDLDependency;
      dep->source = sourceReg;
      // If the token has a source firing, it's not
      // an iteration token, so we want to use the source
      // firing as the source of the dependency.
      VHDLFiring* sourceFiring = 0;
      if (sourceToken) {
	sourceFiring = sourceToken->getSourceFiring();
	if (sourceFiring) {
	  sourceFiring =
	    masterFiringList.vhdlFiringWithName(sourceFiring->getName());
	  dep->source = sourceFiring;
	}
      }
      // If the token has no dest firings, 
      dep->sink = destReg;
      // Need dep to have unique name before putting into list.
      StringList depName = "";
      depName << dep->source->getName() << "," << dep->sink->getName();
      dep->setName(depName);
      if (sourceFiring) {
	dependencyList.put(*dep);
      }
      else {
	iterDependencyList.put(*dep);
      }

      // Add a token to the token list so that it will get processed
      // by the interactive app.
      if (!sourceToken) {
	sourceToken = new VHDLToken;
	sourceToken->setName(sourceReg->getName());
      }
      if (!destToken) {
	destToken = new VHDLToken;
	destToken->setName(destReg->getName());
      }

      tokenList.put(*sourceToken);
      tokenList.put(*destToken);
      
      toggleClock("feedback_clock");

      // Must also create signals for those lines which are neither read nor
      // written by a $ref() - e.g. if more delays than tokens read.
      // However, do not create a signal if it's a wormhole input.
      // Will have created a system port input instead.

      // If no system port by the given name, go ahead and make the signal.
      // This shouldn't cause a problem even if the signals already exist.
      if (sx < arc->lowWrite) {
	signalList.put(sourceName, arc->type);
      }
      if (ix < arc->lowWrite) {
	signalList.put(destName, arc->type);
      }
    }
  }

  // Mux control signals:
  // Generate the correct control signals in the correct order.
  // In the case of simply firing single firings one by one,
  // we just assert that each token is ready after it is ready.
  // This doesn't do much for ports that have single-input muxs.
  // But for state inputs, the assertion of FIRST_ITER_DONE matters.
  //  assertClock("FIRST_ITER_DONE", 1);
  //  assertClock("tokenXready", hashfunction(tokenXname));
  //  assertClock(ctlSignal->getName(), ctlSignal->getControlValue());

  //  toggleClock("start_clock");

  // Iterate through regList and connect registers for each one.
  VHDLRegListIter nextReg(regList);
  VHDLReg* reg;
  while ((reg = nextReg++) != 0) {
    //    cout << "Registername:  " << reg->getName() << "\n";
    if (!reg->getClock()) {
      // Create a dummy clock if the register doesn't have one,
      // because connectRegister needs a clock signal.
      VHDLSignal* newSignal = new VHDLSignal;
      newSignal->setName("DummyClock");
      newSignal->setType("BOOLEAN");
      signalList.put(*newSignal);
      reg->setClock(newSignal);
    }
    connectRegister(reg->getInput(), reg->getOutput(), reg->getClock());
  }

  // Iterate through muxList and connect muxes for each one.
  VHDLMuxListIter nextMux(muxList);
  VHDLMux* mux;
  while ((mux = nextMux++) != 0) {
    //    cout << "MUX with name " << mux->getName() << "\n";
    if (mux->getInputs()->size() > 1) {
      connectMultiplexor(mux->getName(), mux->getInputs(), mux->getOutput(),
			 mux->getControl());
    }
    else if (mux->getInputs()->size() == 1) {
      // Get the name of the only output signal
      StringList outName = mux->getOutput()->getName();
      // Iterate through all firings
      VHDLPort* port = 0;
      VHDLPort* thePort = 0;
      VHDLFiringListIter nextFiring(masterFiringList);
      VHDLFiring* firing;
      while ((firing = nextFiring++) != 0) {
	// Find all ports with the given name
	port = firing->portList->vhdlPortWithName(outName);
	if (port) {
	  thePort = port;
	  thePort->connect(mux->getInputs()->head());
	}
      }
      if (thePort) {
	// Re-connect that port to the only input signal's name
	/*
	cout << "Reconnecting port " << outName;
	cout << " to signal " << mux->getInputs()->head()->getName() << "\n";
	*/
	//	thePort->connect(mux->getInputs()->head());
      }
      else {
	Error::error(outName,
		     ":  Couldn't find a firing with a port of this name");
      }
    }
    else { // mux->getInputs()->size() == 0
      Error::error(mux->getName(), ":  Mux has zero inputs");
    }
  }
}

void ArchTarget :: frameCode() {
  // Initialization so that this part can be repeated.
  firegroupList.initialize();
  
  mainCompDeclList.initialize();
  //  entity_arch.initialize();
  entity_declaration.initialize();
  architecture_body_opener.initialize();
  component_declarations.initialize();
  signal_declarations.initialize();
  component_mappings.initialize();
  architecture_body_closer.initialize();
  configuration_declaration.initialize();

  myCode.initialize();

  // Call the method to interactively rework the arch, if
  // it is provided (by derived targets)
  interact();

  // Update the timing of the controller based on the new
  // timing info from the interactive scheduling.
  
  // Okay, TkSched comes out with each firing having a startTime,
  // and endTime, and a latency.  Start times should determine the
  // times by which control values should have been set up before
  // a given computation can proceed.  End times should determine
  // when output values can be latched.
  {
    // Add the clockCount increment and reset to the head of
    // the controller action code.
    int deadline = 500;
    StringList action = "";

    action << "wait until "
	   << "system_clock'event and system_clock = TRUE;\n";
    action << "if clockCount < " << deadline << " then\n";
    action << "     clockCount := clockCount + 1;\n";
    action << "else\n";
    action << "     clockCount := 0;\n";
    action << "end if;\n";

    action << ctlerAction;
    ctlerAction.initialize();
    ctlerAction << action;

    pulseClock("start_clock", deadline);
    assertValue("FIRST_ITER_DONE", deadline, 1);

    VHDLFiringListIter nextFiring(masterFiringList);
    VHDLFiring* firing;
    while ((firing = nextFiring++) != 0) {
      // Exclude the controller
      StringList ctlName = hashstring(filePrefix);
      ctlName << "controller";
      //      printf("Firing Name:  %s\n", firing->getName());
      //      printf("strcmp:  %d\n",
      //	     (strcmp(ctlName, firing->getName())));

      if (strcmp(ctlName, firing->getName())) {
	// Add a clock at the right time to latch when the firing is done.
	StringList clockName = firing->getName();
	clockName << "_clk";
	int clockTime = firing->endTime;
	pulseClock(clockName, clockTime);

	// Add a new clock signal also.
	VHDLSignal* newSignal = new VHDLSignal;
	newSignal->setName(clockName);
	newSignal->setType("BOOLEAN");
	signalList.put(*newSignal);
      }
    }
  }



  // Add in the entity, architecture, entity declaration, and
  // component mapping for the controller.
  VHDLFiring* ctlerFi = new VHDLFiring;
  StringList ctlName = hashstring(filePrefix);
  ctlName << "controller";
  ctlerFi->setName(ctlName);
  ctlerFi->starClassName = "Sequencer";
  ctlerFi->genericList = ctlerGenericList.newCopy();
  ctlerFi->portList = ctlerPortList.newCopy();
  ctlerFi->signalList = ctlerSignalList.newCopy();
  ctlerFi->variableList = ctlerVariableList.newCopy();
  ctlerFi->action = ctlerAction;
  ctlerAction.initialize();
  ctlerFi->noOutclocking = TRUE;
  ctlerFi->groupNum = -1;

  masterFiringList.put(*ctlerFi);

  // What I really need here is a new list:
  // a list of lists of firings.  Each list
  // of firings in the list represents a group
  // of firings to be executed on the same processor.
  // Knowing this, we can generate the right control
  // and other signals for each firing.  You can't really
  // separate portvar transfers, actions, and varport transfers.

  // Somehow get things into firegroupList.
  // Then deal them out, one firingList at a time
  // For each individual firingList, one firing at a time.

  // Examine each firing, and see what group number it has.
  // Then put it's information into that group.
  // The controller will be handled differently, in its own group.

  {
    // Go through and list each firing's group number.
    VHDLFiringListIter nextFiring(masterFiringList);
    VHDLFiring* firing;
    while ((firing = nextFiring++) != 0) {
      /*
      cout << "Firing:  " << firing->getName() << "\t\t";
      cout << "GroupNum:  " << firing->groupNum << "\n";
      */
    }
  }

  // Re-construct the controller firing's name.
  StringList ctlerName = hashstring(filePrefix);
  ctlerName << "controller";
  const char* ctlString = (const char*) ctlerName;

  {
    VHDLFiringListIter nextFiring(masterFiringList);
    VHDLFiring* firing;
    while ((firing = nextFiring++) != 0) {
      // Construct the firegroup's name.
      // If the firing is the controller, do it differently.
      StringList firegroupName = "FIREGROUP_";
      if (!strcmp(firing->name,ctlString)) {
	firegroupName << "CONTROLLER";
      }
      else {
	if (firing->groupNum >= 0) {
	  firegroupName << firing->groupNum;
	}
	else { /* (firing->groupNum < 0) */
	  firegroupName << "N" << (-(firing->groupNum));
	}
      }
      /*
      cout << "Firing Name:  " << firing->name << "\n";
      cout << "FiregroupName:  " << firegroupName << "\n";
      cout << "\n";
      */

      // See see if the firegroup exists.  If not, create it.
      VHDLFiregroup* firegroup;
      firegroup = firegroupList.vhdlFiregroupWithName(firegroupName);
      if (!firegroup) {
	firegroup = new VHDLFiregroup;
	firegroup->setName(firegroupName);
      }

      // Put the firing in the firegroup's firingList.
      firegroup->firingList->put(*firing);
      // Put the firegroup in the firegroup list.
      firegroupList.put(*firegroup);
    }
  }

  // Unpack the firegroups from firegroupList and
  // generate code for each one.
  int level = 0;
  {
    int newFireNum = 0;
    VHDLFiregroupListIter nextFiregroup(firegroupList);
    VHDLFiregroup* firegroup;
    while ((firegroup = nextFiregroup++) != 0) {
      VHDLFiringList* firingList = firegroup->firingList;

      VHDLGenericList* newGenericList = new VHDLGenericList;
      VHDLPortList* newPortList = new VHDLPortList;
      VHDLVariableList* newVariableList = new VHDLVariableList;

      VHDLFiringListIter nextFiring(*firingList);
      VHDLFiring* firing;
      while ((firing = nextFiring++) != 0) {
	VHDLGenericListIter nextGeneric(*(firing->genericList));
	VHDLGeneric* ge;
	while ((ge = nextGeneric++) != 0) {
	  VHDLGeneric* newGeneric = new VHDLGeneric;
	  newGeneric = ge->newCopy();
	  newGenericList->put(*newGeneric);
	}
	VHDLPortListIter nextPort(*(firing->portList));
	VHDLPort* po;
	while ((po = nextPort++) != 0) {
	  VHDLPort* newPort = new VHDLPort;
	  newPort = po->newCopy();
	  newPortList->put(*newPort);
	}
	VHDLVariableListIter nextVariable(*(firing->variableList));
	VHDLVariable* va;
	while ((va = nextVariable++) != 0) {
	  VHDLVariable* newVariable = new VHDLVariable;
	  newVariable = va->newCopy();
	  newVariableList->put(*newVariable);
	}
      }

      StringList newFireName = firegroup->getName();

      // Add a port for the control signal to select which firing to do.
      VHDLPort* controlPort = new VHDLPort;
      StringList controlName = newFireName;
      controlName << "_CTL";
      controlPort->setName(controlName);
      controlPort->setType("INTEGER");
      controlPort->setDirec("IN");
      // FIXME: Connect to the correct control signal.
      VHDLSignal* iterSignal =
	signalList.vhdlSignalWithName("FIRST_ITER_DONE");
      if (!iterSignal) {
	Error::error("Can't find FIRST_ITER_DONE signal in signalList");
	return;
      }
      controlPort->connect(iterSignal);
      newPortList->put(*controlPort);

      newFireNum++;
      VHDLFiring* combinedFiring = new VHDLFiring;
      combinedFiring->setName(newFireName);
      combinedFiring->starClassName = "MERGED_FIRING";
      combinedFiring->portList = newPortList;
      combinedFiring->genericList = newGenericList;
      combinedFiring->variableList = newVariableList;

      // Iterate through the firing list.
      // Generate entity, architecture for each firing.

      // Still keep around nextFiring list iter, as well
      // as the new combinedFiring, because we really need both.

      nextFiring.reset();
      while ((firing = nextFiring++) != 0) {
	// Begin constructing the firing's code in myCode.
	myCode << "\n-- Firing " << firing->name << "\n";
      }

      myCode << "\n";
      myCode << useLibs;
      myCode << "\n";
      myCode << "entity " << combinedFiring->name << " is\n";

      myCode << addGenericRefs(newGenericList, level);
      myCode << addPortRefs(newPortList, level);

      myCode << indent(level) << "end " << combinedFiring->name << ";\n";
      myCode << "\n";
      myCode << "architecture " << "behavior" << " of "
	     << combinedFiring->name << " is\n";
      myCode << "begin\n";
      myCode << "process\n";

      // To please the Synopsys synthesis:
      // it insists on there being a sensitivity list.
      // FIXME: WAIT! does it?  what is the effect of no sensitivities?
      myCode << addSensitivities(combinedFiring, level);

      myCode << addDeclarations(combinedFiring, level);
      myCode << addVariableRefs(combinedFiring, level);

      myCode << "begin\n";

      // To please the Synopsys simulation:
      // It refuses to initialize ports and variables, and if you
      // don't put a wait statement at the beginning of a process in lieu
      // of explicit initialization for everything (which is unsynthesizable)
      // then you get arithmetic overflow when you hit arithmetic expressions
      // whose inputs are unitiialized variables.
      // Currently we are handling this by doing an explicit global
      // initialization to zero of these values in the simulation
      // command file.

      //   myCode << addWaitStatement(combinedFiring, level);

      // FIXME: Need to simplify this if there is only one firing
      // These core operations of each firing need to be separated by firing
      // and conditioned on the correct control input.
      nextFiring.reset();
      int firingCount = 0;
      VHDLFiring* lastFiring;

      if (firegroup->firingList->size() > 1) {
	while ((firing = nextFiring++) != 0) {
	  // Remember the last firing encountered.
	  lastFiring = firing;
	  if (!firingCount) {
	    myCode << "case " << combinedFiring->name << "_CTL" << " is\n";
	  }
	  firingCount++;
	  myCode << "-- SUB-FIRING " << firingCount << ":  ";
	  myCode << firing->name << "\n";
	  myCode << "when " << firingCount << " => \n";

	  myCode << addPortVarTransfers(firing, level);
	  myCode << addActions(firing, level);
	  myCode << addVarPortTransfers(firing, level);
	}
	if (firingCount) {
	  // Need to cover all cases, so cover others.
	  myCode << "when " << "others" << " => \n";
	  myCode << "end case;\n";
	}
	myCode << "end process;\n";
	myCode << "end behavior;\n";
      }
      else {
	while ((firing = nextFiring++) != 0) {
	  // Remember the last firing encountered.
	  lastFiring = firing;
	  firingCount++;
	  myCode << "-- SUB-FIRING " << firingCount << ":  ";
	  myCode << firing->name << "\n";

	  myCode << addPortVarTransfers(firing, level);
	  myCode << addActions(firing, level);
	  myCode << addVarPortTransfers(firing, level);
	}
	myCode << "end process;\n";
	myCode << "end behavior;\n";
      }

      registerAndMerge(combinedFiring);
    }
  }
  
  buildEntityDeclaration(level);
  buildArchitectureBodyOpener(level);

  component_declarations << addComponentDeclarations(&mainCompDeclList, level);
  component_declarations << addComponentDeclarations(&sourceCompDeclList, level);
  component_declarations << addComponentDeclarations(&muxCompDeclList, level);
  component_declarations << addComponentDeclarations(&regCompDeclList, level);

  signal_declarations << addSignalDeclarations(&signalList, level);

  component_mappings << addComponentMappings(&mainCompDeclList, level);
  component_mappings << addComponentMappings(&sourceCompDeclList, level);
  component_mappings << addComponentMappings(&muxCompDeclList, level);
  component_mappings << addComponentMappings(&regCompDeclList, level);

  buildArchitectureBodyCloser(level);

  //  buildConfigurationDeclaration(level);
  configuration_declaration << addConfigurationDeclarations(&mainCompDeclList, level);
  configuration_declaration << addConfigurationDeclarations(&sourceCompDeclList, level);
  configuration_declaration << addConfigurationDeclarations(&muxCompDeclList, level);
  configuration_declaration << addConfigurationDeclarations(&regCompDeclList, level);

  // Combine all sections of code.
  StringList code = headerComment();

  myCode << "\n-- mux_declarations\n";
  myCode << "\n" << mux_declarations;

  if (systemClock()) {
    myCode << clockGenCode();
  }
  if (registerInts()) {
    myCode << regCode("INTEGER");
  }
  if (registerReals()) {
    myCode << regCode("REAL");
  }
  if (multiplexorInts()) {
    myCode << muxCode("INTEGER");
  }
  if (multiplexorReals()) {
    myCode << muxCode("REAL");
  }
  if (sourceInts()) {
    myCode << sourceCode("INTEGER");
  }
  if (sourceReals()) {
    myCode << sourceCode("REAL");
  }
  
  myCode << "\n-- entity_declaration\n";
  myCode << "\n" << entity_declaration;
  myCode << "\n-- architecture_body_opener\n";
  myCode << "\n" << architecture_body_opener;
  myCode << "\n-- component_declarations\n";
  myCode << "\n" << component_declarations;
  myCode << "\n-- signal_declarations\n";
  myCode << "\n" << signal_declarations;
  myCode << "\n" << "begin";
  myCode << "\n-- component_mappings\n";
  myCode << "\n" << component_mappings;
  myCode << "\n-- architecture_body_closer\n";
  myCode << "\n" << architecture_body_closer;
  //  myCode << "\n-- configuration_declaration\n";
  //  myCode << "\n" << configuration_declaration;

  // Prepend the header, declarations, and initialization.
  prepend(code, myCode);
}

// Write the code to a file.
void ArchTarget :: writeCode() {
  SimVSSTarget::writeCode();
}

// Compile the code.
int ArchTarget :: compileCode() {
  return SimVSSTarget::compileCode();
}

// Run the code.
int ArchTarget :: runCode() {
  return SimVSSTarget::runCode();
}

// Merge the Star's signal list with the Target's signal list.
void ArchTarget :: mergeSignalList(VHDLSignalList* starSignalList) {
  VHDLSignalListIter starSignalNext(*starSignalList);
  VHDLSignal* nStarSignal;
  // Scan through the list of signals from the star firing.
  while ((nStarSignal = starSignalNext++) != 0) {
    // Search for a match from the existing list.
    if (!(signalList.inList(nStarSignal))) {
      // Allocate memory for a new VHDLSignal and put it in the list.
      VHDLSignal* newSignal = new VHDLSignal;
      newSignal = nStarSignal->newCopy();
      signalList.put(*newSignal);
    }
  }
}

// Register temporary variable reference.
void ArchTarget :: registerTemp(const char* name, const char* type) {
    VHDLVariable* newVar = new VHDLVariable;
    newVar->setName(name);
    newVar->setType(type);

    firingVariableList.put(*newVar);
}

// Register PortHole reference.
void ArchTarget :: registerPortHole(VHDLPortHole* port, const char* dataName,
				      int firing,
				      int tokenNum/*=-1*/,
				      const char* part/*=""*/) {
  // WARNING: DON'T get confused between "port", which is a VHDLPortHole*,
  // and "newPort" which is a VHDLPort*.  Need to improve this.

  // I want to pass along the offset info as well as whether it's
  // a read or a write so I can keep tabs on the production window and
  // the read window, and then add in registers at the end to handle
  // storage of intermediate tokens between iterations.
  // Also, must do special things if it's a wormhole input.

  // The registry keeps track of all refed arcs, and their min/max R/W offsets.
  registerArcRef(port, tokenNum);

  /*
  cout << "registerPortHole:  dataName: " << dataName;
  cout << "                   portName: " << port->name() << "\n";
  */
  /*
  cout << "  tokenNum:  " << tokenNum << "\n";
  */

  StringList ref = dataName;
  ref << part;
  // Name for the variable to use.
  StringList varName = ref;
  // Name for the token to use.
  StringList tokenName = ref;
  // Name for ports that are a source of this token to use.
  StringList sourcePortName = ref;
  sourcePortName << "_OUT";
  // Name for ports that are a dest of this token to use.
  StringList destPortName = ref;
  destPortName << "_IN";
  // Now test for direction and set portName accordingly.
  StringList portName;
  if (port->isItInput()) {
    //    cout << "It's an input\n";
    portName = destPortName;
  }
  if (port->isItOutput()) {
    //    cout << "It's an output\n";
    portName = sourcePortName;
  }
  //  cout << "Port's name shall be:  " << portName << "\n";

  // Data clock name.  Needs to be the name of the firing, not the star.
  StringList clockName = sanitize(port->parent()->fullName());
  clockName << "_F" << firing;
  clockName << "_clk";

  // Look for a port with the given name already in the list.
  VHDLPort* existPort = firingPortList.vhdlPortWithName(portName);
  if (!existPort) {
    // Create a port and a port mapping for this firing entity.
    VHDLPort* newPort = new VHDLPort;
    newPort->setName(portName);
    newPort->setType(port->dataType());
    newPort->setDirec(port->direction());
    newPort->setFire(currentFiring);

    VHDLVariable* newVar = new VHDLVariable;
    newVar->setName(varName);
    newVar->setType(port->dataType());

    newPort->setVar(newVar);

    firingVariableList.put(*newVar);

    // If it's an input port, find the token holding the data.
    if (newPort->isItInput()) {
      // Find the token, if it already exists.
      VHDLToken* existToken = tokenList.vhdlTokenWithName(tokenName);
      if (existToken) {
	// If tokenNum is negative, and token exists, must have
	// already been referenced.  Need to add another dest firing.
	// Also need to set newPort's token to this token.
	if (tokenNum < 0) {
	  existToken->addDestFiring(currentFiring);
	  newPort->setToken(existToken);

	  // Don't add a dependency in this case - reading a delay token.
	}
	else { // tokenNum >= 0
	  // existToken exists with portName as name, so associate with it.
	  existToken->addDestFiring(currentFiring);
	  newPort->setToken(existToken);

	  // Add a dependency in this case.
	  {
	    // Register a new VHDLDependency.
	    VHDLDependency* dep = new VHDLDependency;
	    // Port is IN, so source is existToken's port's firing.
	    VHDLFiring* sourceFiring = existToken->getSourceFiring();
	    if(!sourceFiring) {
	      Error::abortRun(existToken->name,
			      " has NULL sourceFiring in registerPort");
	    }
	    dep->source = sourceFiring;

	    // Port is IN, so sink is currentFiring.
	    dep->sink = currentFiring;

	    // Need dep to have unique name before putting into list.
	    StringList depName = "";
	    depName << dep->source->getName() << "," << dep->sink->getName();
	    dep->setName(depName);
	    dependencyList.put(*dep);
	  }
	}
      }
      else { // !existToken
	// tokenNum is negative, therefore it couldn't have been output from
	// a firing, so create a new token to input from.
	if (tokenNum < 0) {
	  VHDLToken* newToken = new VHDLToken;
	  newToken->setName(tokenName);
	  newToken->setType(port->dataType());

	  newToken->addDestFiring(currentFiring);
	  newPort->setToken(newToken);

	  tokenList.put(*newToken);
	}
	else { // tokenNum >= 0
	  // tokenNum not negative: token should already exist, but doesn't.
	  Error::abortRun(portName,
			  ": Need to associate with token for data which ",
			  "hasn't been generated yet - firing order error?");
	  return;
	}
      }
    }
    // If it's an output port, create a new output token.
    if (newPort->isItOutput()) {
      VHDLToken* newToken = new VHDLToken;
      newToken->setName(tokenName);
      newToken->setType(port->dataType());
      newToken->clockName = clockName;

      // Add a new clock signal also.
      VHDLSignal* newSignal = new VHDLSignal;
      newSignal->setName(clockName);
      newSignal->setType("BOOLEAN");
      signalList.put(*newSignal);

      newToken->setSourceFiring(currentFiring);
      newPort->setToken(newToken);

      tokenList.put(*newToken);
    }

    // Finally, put the new port in the firing's port list.
    firingPortList.put(*newPort);
  }

  // Only provide a toggled clock for firing if data is output.
  if (port->isItOutput()) {
    //    toggleClock(clockName);
  }

  // FIXME: Wormholes not currently supported
  // For wormhole inputs, create a system port.
  // But for delayed values of wormhole inputs, do not create another port.
  if ((port->atBoundary()) && (tokenNum >= 0)) {
    // Signal an error: we won't support wormholes for now:
    Error::error(*port, "is at a wormhole boundary: Not currently supported");
    // Port at wormhole boundary, so create a system port instead of a signal.
    //    systemPortList.put(portName, port->direction(), port->dataType());
  }
  // For local inputs, create a signal.
  else {
    // The following is commented out: we only create and register
    // a new signal when the port that generates the data is created.
    //    firingSignalList.put(sinkName, port->dataType());
  }

}

// Register the State reference.
void ArchTarget :: registerState(State* state, const char* varName,
				   int thisFiring/*=-1*/, int pos/*=-1*/) {
  StringList ref = varName;
  StringList stType = stateType(state);
  StringList root;
  StringList initVal;

  // Initialize the state and check for inconsistencies.
  if (pos >= 0) {
    if (!(state->isArray())) {
      Error::error(*state, " reference of non-arrayState as an arrayState");
    }
    else {
      if (pos >= state->size()) {
	Error::error(*state, " attempt to reference arrayState past its size");
      }
      else {
	state->initialize();
	ref << "_P" << pos;
	if (state->isA("FloatArrayState")) {
	  initVal << (*((FloatArrayState *) state))[pos];
	}
	else if (state->isA("IntArrayState")) {
	  initVal << (*((IntArrayState *) state))[pos];
	}
	else {
	  Error::error(*state, "is not a Float or Int ArrayState");
	}
      }
    }
  }
  else {
    state->initialize();
    initVal << state->currentValue();
  }
  
  // If state is constant, according to its attributes, do things
  // a bit differently: have all references refer only to the single
  // constant source, rather than passing through values from one firing
  // to the next, which is useless and restrictive.
  // Also, there should be no output on which to pass on the value,
  // since it isn't changed by this firing.

  // FIXME: Remove redundancy of constState and listState->constant.
  int constState;
  if (state->attributes() & AB_CONST) {
    constState = 1;
  }
  else {
    constState = 0;
  }

  // Root is ref, without marking for any particular firing.
  root = ref;

  // Names for the inVar and outVar to use.
  StringList inVarName = ref;
  StringList outVarName = ref;
  StringList inPortName = sanitize(ref);
  StringList outPortName = sanitize(ref);
  StringList inTokenName = ref;
  StringList outTokenName = ref;
  if (!constState) {
    inTokenName << "_F" << (thisFiring-1);
    outTokenName << "_F" << thisFiring;
  }
  inPortName = inTokenName;
  inPortName << "_IN";
  outPortName = outTokenName;
  outPortName << "_OUT";

  // Data clock name.  Needs to be the name of the firing, not the star.
  StringList clockName = sanitize(state->parent()->fullName());
  clockName << "_F" << thisFiring;
  clockName << "_clk";

  // look for state in stateList
  // if don't find it, enter it with -1 as state->lastFiring
  int stateLastFiring = -1;
  VHDLState* listState = stateList.vhdlStateWithName(root);
  if (!listState) {
    // Create a new VHDLState and put it in the list.
    listState = new VHDLState;
    listState->setName(root);
    listState->firstRef = inTokenName;
    listState->firstFiringName = currentFiring->getName();
    listState->lastRef = outTokenName;
    listState->initVal = initVal;
    listState->type = stType;
    listState->lastFiring = -1;
    stateList.put(*listState);
  }
  // set listState to the one that's actually in the list.
  listState = stateList.vhdlStateWithName(root);
  stateLastFiring = listState->lastFiring;

  int isFirstStateRef = (stateLastFiring == -1);
  
  // If this is the first reference to this state in this firing
  // need to put it in the proper lists and make connections.
  if (listState->lastFiring != thisFiring) {
    // Inside this if clause, this state is being referenced
    // for the first time in this firing, perhaps the first time ever.

    // Create a new port and variable mapping.
    VHDLVariable* inVar = new VHDLVariable;
    inVar->setName(inVarName);
    inVar->setType(stType);

    VHDLPort* inPort = new VHDLPort;
    inPort->setName(inPortName);
    inPort->setType(stType);
    inPort->setDirec("IN");
    inPort->setVar(inVar);
    inPort->setFire(currentFiring);

    firingPortList.put(*inPort);

    firingVariableList.put(*inVar);

    if (constState) {
      listState->constRefFiringList << currentFiring->getName();

      // NEW: NO LONGER CREATE A TOKEN FOR CONST STATES
      /*
      listState->constant = 1;
      if (isFirstStateRef) {
	// This is the first state ref, so create a new token.
	listState->firstRef = inTokenName;
	listState->firstFiringName = currentFiring->getName();
	listState->lastRef = inTokenName;

	VHDLToken* inToken = new VHDLToken;
	inToken->setName(inTokenName);
	inToken->setType(stType);

	inToken->addDestFiring(currentFiring);
	inPort->setToken(inToken);

	tokenList.put(*inToken);
      }
      if (!isFirstStateRef) {
	// Need to find the existing token to associate with.
	// That token should have been created during the previous firing's
	// first reference to the constant state.
	VHDLToken* existToken = tokenList.vhdlTokenWithName(inTokenName);
	if (!existToken) {
	  Error::abortRun(inTokenName,
			  ": Not first const state ref, but can't",
			  " find any existing source token created for it");
	  return;
	}
	existToken->addDestFiring(currentFiring);
	inPort->setToken(existToken);
      }
      */
    }

    if (!constState) {
      listState->constant = 0;

      // Create a new port and variable mapping.
      VHDLVariable* outVar = new VHDLVariable;
      outVar->setName(outVarName);
      outVar->setType(stType);

      VHDLPort* outPort = new VHDLPort;
      outPort->setName(outPortName);
      outPort->setType(stType);
      outPort->setDirec("OUT");
      outPort->setFire(currentFiring);
      outPort->setVar(outVar);

      // Create a new output token to carry the updated state value.
      VHDLToken* outToken = new VHDLToken;
      outToken->setName(outTokenName);
      outToken->setType(stType);
      outToken->clockName = clockName;

      // Add a new clock signal also.
      VHDLSignal* newSignal = new VHDLSignal;
      newSignal->setName(clockName);
      newSignal->setType("BOOLEAN");
      signalList.put(*newSignal);

      outToken->setSourceFiring(currentFiring);
      outPort->setToken(outToken);

      tokenList.put(*outToken);

      if (isFirstStateRef) {
	// This is the first state ref, so create a new token.
	VHDLToken* inToken = new VHDLToken;
	inToken->setName(inTokenName);
	inToken->setType(stType);

	inToken->addDestFiring(currentFiring);
	inPort->setToken(inToken);

	tokenList.put(*inToken);
      }
      if (!isFirstStateRef) {
	// Need to find the existing token to associate with.
	// That token should have been created during the previous firing's
	// reference to the output state token.
	VHDLToken* existToken = tokenList.vhdlTokenWithName(inTokenName);
	if (!existToken) {
	  Error::abortRun(inTokenName,
			  ": Not first nonconst state ref, but can't",
			  " find any existing output token created for it");
	  return;
	}
	existToken->addDestFiring(currentFiring);
	inPort->setToken(existToken);

	{
	  // Register a new VHDLDependency.
	  VHDLDependency* dep = new VHDLDependency;
	  // Not first firing state ref, so source is
	  // existToken's port's firing.
	  VHDLFiring* sourceFiring = existToken->getSourceFiring();
	  if(!sourceFiring) {
	    Error::abortRun(existToken->name,
			    " has NULL sourceFiring in registerState");
	  }
	  dep->source = sourceFiring;

	  // Reading state token in, so sink is currentFiring.
	  dep->sink = currentFiring;

	  // Need dep to have unique name before putting into list.
	  StringList depName = "";
	  depName << dep->source->getName() << "," << dep->sink->getName();
	  dep->setName(depName);
	  dependencyList.put(*dep);
	}
      }

      // Finally, put the outPort in the firing's port list.
      firingPortList.put(*outPort);

      // FIXME: Stil need to get that clock timing right.
      //      toggleClock(clockName);
    }

    // Reset state's lastRef name.
    listState->lastRef = outTokenName;
  }

  // Reset state's lastFiring tag just before exiting.
  listState->lastFiring = thisFiring;
  listState->lastFiringName = currentFiring->getName();
}

// Connect a source of the given value to the given signal.
void ArchTarget :: connectSource(StringList initVal, VHDLSignal* initSignal)
{
  if (!initSignal) {
    Error::abortRun(*this, "connectSource passed a null initSignal");
    return;
  }

  StringList type = initSignal->getType();

  // Set the flag indicating sources are needed.
  if (!strcmp(type,"INTEGER")) {
    setSourceInts();
  }
  if (!strcmp(type,"REAL")) {
    setSourceReals();
  }

  StringList label = initSignal->name;
  label << "_SOURCE";
  StringList name = "Source";
  name << "_" << type;

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;

  VHDLGeneric* valueGen = new VHDLGeneric("value", type, "", initVal);
  genList->put(*valueGen);

  VHDLPort* outPort = new VHDLPort("output", type, "OUT", "", NULL);
  outPort->connect(initSignal);
  portList->put(*outPort);

  sourceCompDeclList.put(label, portList, genList,
			 name, portList, genList);
}

// Connect a multiplexor between the given input and output signals.
void ArchTarget :: connectMultiplexor(StringList muxName,
				      VHDLSignalList* inSignals,
				      VHDLSignal* outSignal,
				      VHDLSignal* ctlSignal) {
  if (!inSignals) {
    Error::abortRun(*this, "connectMultiplexor passed a null inSignals");
    return;
  }
  if (!outSignal) {
    Error::abortRun(*this, "connectMultiplexor passed a null outSignal");
    return;
  }
  if (!ctlSignal) {
    Error::abortRun(*this, "connectMultiplexor passed a null ctlSignal");
    return;
  }

  // FIXME: Need to check the type of all input signals.
  // FIXME: Need to test for control value for all input signals.
  // FIXME: Need to test for uniqueness of control value of all input signals.

  /*
  if ((strcmp(outSignal->getType(),inSignal->getType())) ||
      (strcmp(outSignal->getType(),ctlSignal->getType()))) {
    Error::error(outSignal->name,
		 ": connectMultiplexor: Types of signals do not match");
    return;
  }
  */
  StringList type = outSignal->getType();

  // Create the code to implement the multiplexor.
  StringList codeList;
  codeList << "\n";
  codeList << "     -- Mux_" << muxName << " : Multiplexor of type " << type << "\n";
  codeList << "entity Mux_" << muxName << " is\n";
  codeList << "	port(\n";
  codeList << "		control: IN INTEGER;\n";
  {
    VHDLSignalListIter nextSignal(*inSignals);
    VHDLSignal* signal;
    while ((signal = nextSignal++) != 0) {
      codeList << "		" << signal->getName() << ": IN " << type << ";\n";
    }
  }
  codeList << "		output: OUT " << type << "\n";
  codeList << "	);\n";
  codeList << "end Mux_" << muxName << ";\n";
  codeList << "\n";
  codeList << "architecture behavior of Mux_" << muxName << " is\n";
  codeList << "	begin\n";
  codeList << "		process (control";
  {
    VHDLSignalListIter nextSignal(*inSignals);
    VHDLSignal* signal;
    while ((signal = nextSignal++) != 0) {
      codeList << ", " << signal->getName();
    }
  }
  codeList << ")\n";
  codeList << "		begin\n";
  codeList << "		  case control is\n";
  {
    VHDLSignalListIter nextSignal(*inSignals);
    VHDLSignal* signal;
    while ((signal = nextSignal++) != 0) {
      codeList << "		";
      codeList << "when "<< signal->getControlValue();
      codeList << " =>  " << "output <= " << signal->getName() << ";\n";
    }
  }
  codeList << "		";
  codeList << "when " << "others";
  codeList << " =>  " << "\n";
  codeList << "		  end case;\n";

  codeList << "		end process;\n";
  codeList << "end behavior;\n";
  codeList << "\n";

  // Putting this in myCode is fatal, when myCode is getting reset.
  //  myCode << codeList;
  mux_declarations << codeList;

  StringList label = outSignal->name;
  label << "_MUX";
  StringList name = "Mux";
  name << "_" << muxName;

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;

  /*
  VHDLSignal* controlSignal = ctlerSignalList.vhdlSignalWithName("control");
  if (!controlSignal) {
    controlSignal = new VHDLSignal("control", "boolean", NULL);
    ctlerSignalList.put(*controlSignal);
  }
  */

  {
    VHDLSignalListIter nextSignal(*inSignals);
    VHDLSignal* signal;
    while ((signal = nextSignal++) != 0) {
      VHDLPort* newPort = new VHDLPort(signal->getName(), type, "IN", signal->getName(), NULL);
      newPort->connect(signal);
      portList->put(*newPort);
    }
  }

  VHDLPort* outPort = new VHDLPort("output", type, "OUT", outSignal->getName(), NULL);
  outPort->connect(outSignal);
  portList->put(*outPort);

  VHDLPort* controlPort = new VHDLPort("control", "INTEGER", "IN", ctlSignal->getName(), NULL);
  controlPort->connect(ctlSignal);
  portList->put(*controlPort);

  // If using a system clock generator, then need a signal.
  if (systemClock()) {
    signalList.put("system_clock", "boolean");
    connectClockGen("system_clock");
  }
  else {
    systemPortList.put("system_clock", "boolean", "IN");
  }
  muxCompDeclList.put(label, portList, genList,
		       name, portList, genList);
}

// Connect a register between the given input and output signals.
void ArchTarget :: connectRegister(VHDLSignal* inSignal,
				     VHDLSignal* outSignal,
				     VHDLSignal* clkSignal) {
  if (!inSignal) {
    Error::abortRun(*this, "connectRegister passed a null inSignal");
    return;
  }
  if (!outSignal) {
    Error::abortRun(*this, "connectRegister passed a null outSignal");
    return;
  }
  if (!clkSignal) {
    Error::abortRun(*this, "connectRegister passed a null clkSignal");
    return;
  }

  if (strcmp(outSignal->getType(),inSignal->getType())) {
    Error::error(outSignal->name,
		 ": connectRegister: Types of signals do not match");
    return;
  }
  StringList type = outSignal->getType();

  // Set the flag indicating registers are needed.
  if (!strcmp(type,"INTEGER")) {
    setRegisterInts();
  }
  if (!strcmp(type,"REAL")) {
    setRegisterReals();
  }

  StringList label = outSignal->name;
  label << "_REG";
  StringList name = "Reg";
  name << "_" << type;

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;

  VHDLPort* inPort = new VHDLPort("D", type, "IN", "", NULL);
  VHDLPort* outPort = new VHDLPort("Q", type, "OUT", "", NULL);
  VHDLPort* clkPort = new VHDLPort("C", "boolean", "IN", "", NULL);

  inPort->connect(inSignal);
  outPort->connect(outSignal);
  clkPort->connect(clkSignal);

  portList->put(*inPort);
  portList->put(*outPort);
  portList->put(*clkPort);


  ctlerPortList.put(clkSignal->name, "boolean", "OUT", clkSignal->name, NULL);
  ctlerPortList.put("system_clock", "boolean", "IN", "system_clock", NULL);

  // If using a system clock generator, then need a signal.
  if (systemClock()) {
    signalList.put("system_clock", "boolean");
    connectClockGen("system_clock");
  }
  else {
    systemPortList.put("system_clock", "boolean", "IN");
  }
  regCompDeclList.put(label, portList, genList,
		      name, portList, genList);
}

// Connect a clock generator driving the given signal.
void ArchTarget :: connectClockGen(StringList clkName) {
  // Set the flag indicating clock generator used.
  setSystemClockUsed();

  StringList label = clkName;
  label << "_Clock";
  StringList name = "ClockGen";

  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;

  portList->put("system_clock", "boolean", "OUT", clkName, NULL);
  portList->put("iter_clock", "boolean", "IN", "iter_clock", NULL);

  mainCompDeclList.put(label, portList, genList, name, portList, genList);
}

ISA_FUNC(ArchTarget,SimVSSTarget);

// Method called by comm stars to place important code into structure.
void ArchTarget :: registerComm(int direction, int pairid, int numxfer,
				  const char* dtype) {
  // direction == 0 --> C2V ; direction == 1 --> V2C.
  // Create a string with the right VHDL data type
  StringList vtype = "";
  StringList name = "";
  if (strcmp(dtype, "INT") == 0) {
    vtype = "INTEGER";
    if (direction) {
      name = "V2Cinteger";
    }
    else {
      name = "C2Vinteger";
    }
  }
  else if (strcmp(dtype, "FLOAT") == 0) {
    vtype = "REAL";
    if (direction) {
      name = "V2Creal";
    }
    else {
      name = "C2Vreal";
    }
  }
  else {
    Error::abortRun(*this, dtype, ": type not supported");
    return;
  }
  
  // Construct unique label and signal names and put comp in main list
  StringList label;
  StringList goName, dataName, doneName;
  StringList rootName = name;
  rootName << pairid;

  label << rootName;
  goName << rootName << "_go";
  dataName << rootName << "_data";
  doneName << rootName << "_done";
  
  VHDLGenericList* genList = new VHDLGenericList;
  VHDLPortList* portList = new VHDLPortList;
  
  genList->put("pairid", "INTEGER", "", pairid);
  genList->put("numxfer", "INTEGER", "", numxfer);

  portList->put("go", "STD_LOGIC", "IN", goName, NULL);
  if (direction) {
    firingPortList.put(dataName, vtype, "OUT", dataName, NULL);
    firingSignalList.put(dataName, vtype, NULL);
    portList->put("data", vtype, "IN", dataName, NULL);
  }
  else {
    portList->put("data", vtype, "OUT", dataName, NULL);
    firingSignalList.put(dataName, vtype, NULL);
    firingPortList.put(dataName, vtype, "IN", dataName, NULL);
  }
  portList->put("done", "STD_LOGIC", "OUT", doneName, NULL);

  mainCompDeclList.put(label, portList, genList, name, portList, genList);

  StringList startName;
  StringList endName;
  startName << rootName << "_start";
  endName << rootName << "_end";

  // Also add to port list of controller.
  ctlerPortList.put(startName, "STD_LOGIC", "OUT", startName, NULL);
  ctlerSignalList.put(startName, "STD_LOGIC", NULL);
  firingSignalList.put(startName, "STD_LOGIC", NULL);
  firingPortList.put(startName, "STD_LOGIC", "IN", startName, NULL);
  firingPortList.put(goName, "STD_LOGIC", "OUT", goName, NULL);
  firingSignalList.put(goName, "STD_LOGIC", NULL);
  firingSignalList.put(doneName, "STD_LOGIC", NULL);
  firingPortList.put(doneName, "STD_LOGIC", "IN", doneName, NULL);
  firingPortList.put(endName, "STD_LOGIC", "OUT", endName, NULL);
  firingSignalList.put(endName, "STD_LOGIC", NULL);
  ctlerSignalList.put(endName, "STD_LOGIC", NULL);
  ctlerPortList.put(endName, "STD_LOGIC", "IN", endName, NULL);

  ctlerAction << startName << " <= '0';\n";
  preSynch << "wait on " << startName << "'transaction;\n";
  postSynch << endName << " <= '0';\n";
  ctlerAction << "wait on " << endName << "'transaction;\n";
}

// Add in sensitivity list of input ports.
// Do this explicitly for sake of synthesis.
StringList ArchTarget :: addSensitivities(VHDLFiring* firing, int level) {
  StringList all;
    StringList opener, body, closer;

    level++;
    opener << indent(level) << "(\n";

    int sensCount = 0;
      // Special exception to avoid sensitivity list for controller
      // which will contain wait statements.  Synopsys complains that
      // it can't synthesize blocks with wait statements if they also
      // have sensitivity lists.
      StringList ctlName = "FIREGROUP_";
      ctlName << "CONTROLLER";
      const char* ctlString = (const char*) ctlName;
      if (!strcmp(firing->name,ctlString)) return all;
      // Another exception for those firings that wish to disable
      // sensitivity lists, usually due to needing wait statements
      // within the firing, such as for send/receive synchronization.
      if (firing->noSensitivities) return all;
      // Otherwise, proceed.
      if ((*(firing->portList)).head()) {
	VHDLPortListIter nextPort(*(firing->portList));
	VHDLPort* nport;
	while ((nport = nextPort++) != 0) {
	  if (nport->isItInput()) {
	    level++;
	    if (sensCount) {
	      body << ",\n";
	    }
	    body << indent(level) << nport->name;
	    sensCount++;
	    level--;
	  }
	}
      }
    
    closer << "\n";
    closer << indent(level) << ")\n";
    level--;

    if (sensCount) {
      all << opener << body << closer;
    }
  return all;
}

// Add in wait statement with list of input ports.
// Do this explicitly for sake of simulation.
StringList ArchTarget :: addWaitStatement(VHDLFiring* firing, int level) {
  StringList all;
    StringList opener, body, closer;

    level++;
    opener << indent(level) << "wait on\n";

    int sensCount = 0;
      // Special exception to avoid sensitivity list for controller
      // which will contain wait statements.  Synopsys complains that
      // it can't synthesize blocks with wait statements if they also
      // have sensitivity lists.  Also don't want more than one wait
      // statement.
      StringList ctlName = hashstring(filePrefix);
      ctlName << "controller";
      const char* ctlString = (const char*) ctlName;
      if (!strcmp(firing->name,ctlString)) return all;
      if ((*(firing->portList)).head()) {
	VHDLPortListIter nextPort(*(firing->portList));
	VHDLPort* nport;
	while ((nport = nextPort++) != 0) {
	  if (nport->isItInput()) {
	    level++;
	    if (sensCount) {
	      body << ",\n";
	    }
	    body << indent(level) << nport->name;
	    sensCount++;
	    level--;
	  }
	}
      }
    
    closer << "\n";
    closer << indent(level) << ";\n";
    level--;

    if (sensCount) {
      all << opener << body << closer;
    }
  return all;
}

// Add in variable refs here from variableList.
StringList ArchTarget :: addVariableRefs(VHDLFiring* firing, int level) {
  StringList all;
      all << addVariableDecls(firing->variableList, level, 1);
  return all;
}

// Add in port to variable transfers here from portVarList.
StringList ArchTarget :: addPortVarTransfers(VHDLFiring* firing,
					       int /*level*/) {
  StringList all;
    StringList body;

    int portCount = 0;
      VHDLPortListIter nextPort(*(firing->portList));
      VHDLPort* nPort;
      while ((nPort = nextPort++) != 0) {
	if (nPort->isItInput() && nPort->variable) {
	  body << nPort->variable->name << " := " << nPort->name << ";\n";
	  portCount++;
	}
      }
    
    if (portCount) {
      all << body;
    }
  return all;
}

// Add in firing declarations here.
StringList ArchTarget :: addDeclarations(VHDLFiring* firing, int /*level*/) {
  StringList all;
    StringList body;

    int declsCount = 0;
      body << firing->decls;
      declsCount++;
    
    if (declsCount) {
      all << body;
    }
  return all;
}

// Add in firing actions here.
StringList ArchTarget :: addActions(VHDLFiring* firing, int /*level*/) {
  StringList all;
    StringList body;

    int actionCount = 0;
      body << firing->action;
      actionCount++;

    if (actionCount) {
      all << body;
    }
  return all;
}

// Add in variable to port transfers here from varPortList.
StringList ArchTarget :: addVarPortTransfers(VHDLFiring* firing,
					       int /*level*/) {
  StringList all;
    StringList body;

    int portCount = 0;
      VHDLPortListIter nextPort(*(firing->portList));
      VHDLPort* nPort;
      while ((nPort = nextPort++) != 0) {
	if (nPort->isItOutput() && nPort->variable) {
	  body << nPort->name << " <= " << nPort->variable->name << ";\n";
	  portCount++;
	}
      }
    
    if (portCount) {
      all << body;
    }
  return all;
}

// Register compDecls and merge signals.
void ArchTarget :: registerAndMerge(VHDLFiring* firing) {
  StringList firingLabel = firing->name;
  firingLabel << "_proc";
  StringList firingName = firing->name;
  
  // tack all onto master lists
  VHDLPortList* masterPortList = new VHDLPortList;
  VHDLGenericList* masterGenericList = new VHDLGenericList;
  VHDLSignalList* masterSignalList = new VHDLSignalList;

    VHDLPortListIter nextPort(*(firing->portList));
    VHDLPort* po;
    while ((po = nextPort++) != 0) {
      VHDLPort* newPort = new VHDLPort;
      newPort = po->newCopy();
      masterPortList->put(*newPort);
    }
    VHDLGenericListIter nextGeneric(*(firing->genericList));
    VHDLGeneric* ge;
    while ((ge = nextGeneric++) != 0) {
      VHDLGeneric* newGeneric = new VHDLGeneric;
      newGeneric = ge->newCopy();
      masterGenericList->put(*newGeneric);
    }
    VHDLSignalListIter nextSignal(*(firing->signalList));
    VHDLSignal* si;
    while ((si = nextSignal++) != 0) {
      VHDLSignal* newSignal = new VHDLSignal;
      newSignal = si->newCopy();
      masterSignalList->put(*newSignal);
    }

  mainCompDeclList.put(firingLabel, masterPortList, masterGenericList,
		       firingName, masterPortList, masterGenericList);
  mergeSignalList(masterSignalList);
}

// Generate the entity_declaration.
void ArchTarget :: buildEntityDeclaration(int level) {
  entity_declaration << "-- firing use clauses\n";
  entity_declaration << "library SYNOPSYS,IEEE,PTVHDLSIM;\n";
  entity_declaration << "use SYNOPSYS.ATTRIBUTES.all;\n";
  entity_declaration << "use IEEE.STD_LOGIC_1164.all;\n";
  entity_declaration << "use std.textio.all;\n";

  entity_declaration << "entity " << (const char*) filePrefix << " is\n";
  if (systemPortList.head()) {
    level++;
    entity_declaration << indent(level) << "port(\n";
    VHDLPortListIter nextPort(systemPortList);
    VHDLPort* nport;
    int portCount = 0;
    while ((nport = nextPort++) != 0) {
      level ++;
      if (portCount) {
	entity_declaration << ";\n";
      }
      entity_declaration << indent(level) << nport->name << ": "
			 << nport->direction << " " << nport->type;
      portCount++;
      level--;
    }
    entity_declaration << "\n";
    entity_declaration << indent(level) << ");\n";
    level--;
  }
  entity_declaration << "end " << (const char*) filePrefix << ";\n";
}

// Generate the architecture_body_opener.
void ArchTarget :: buildArchitectureBodyOpener(int /*level*/) {
  architecture_body_opener << "architecture " << "structure" << " of "
			   << (const char*) filePrefix << " is\n";
}

// Add in component declarations here from mainCompDeclList.
void ArchTarget :: buildComponentDeclarations(int level) {
  Error::warn("buildComponentDeclarations", ": Don't call me anymore!");

  // HashTable to keep track of which components already declared.
  HashTable myTable;
  myTable.clear();

  VHDLCompDeclListIter nextCompDecl(mainCompDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    if (!(myTable.hasKey(compDecl->type))) {
      myTable.insert(compDecl->type, compDecl);

      level++;
      component_declarations << indent(level) << "component " << compDecl->type
			     << "\n";

      // Add in generic refs here from genList.
      if (compDecl->genList->head()) {
	level++;
	component_declarations << indent(level) << "generic(\n";
	VHDLGenericListIter nextGen(*(compDecl->genList));
	VHDLGeneric* ngen;
	int genCount = 0;
	while ((ngen = nextGen++) != 0) {
	  level++;
	  if (genCount) {
	    component_declarations << ";\n";
	  }
	  component_declarations << indent(level) << ngen->name << ": "
				 << ngen->type;
	  if (ngen->defaultVal.length() > 0) {
	    component_declarations << " := " << ngen->defaultVal;
	  }
	  genCount++;
	  level--;
	}
	component_declarations << "\n";
	component_declarations << indent(level) << ");\n";
	level--;
      }
    
      // Add in port refs here from portList.
      if (compDecl->portList->head()) {
	level++;
	component_declarations << indent(level) << "port(\n";
	VHDLPortListIter nextPort(*(compDecl->portList));
	VHDLPort* nport;
	int portCount = 0;
	while ((nport = nextPort++) != 0) {
	  level++;
	  if (portCount) {
	    component_declarations << ";\n";
	  }
	  component_declarations << indent(level) << nport->name << ": "
				 << nport->direction << " " << nport->type;
	  portCount++;
	  level--;
	}
	component_declarations << "\n";
	component_declarations << indent(level) << ");\n";
	level--;
      }
    
      component_declarations << indent(level) << "end component;\n";
      level--;
    }
  }
}

// Generate the architecture_body_closer.
void ArchTarget :: buildArchitectureBodyCloser(int /*level*/) {
  architecture_body_closer << "end structure;\n";
}

// Add in configuration declaration here from mainCompDeclList.
void ArchTarget :: buildConfigurationDeclaration(int level) {
  // HashTable to keep track of which components already configured.
  HashTable myTable;
  myTable.clear();

  configuration_declaration << "configuration " << (const char*) filePrefix
			    << "_parts" << " of " << (const char*) filePrefix
			    << " is\n";
  configuration_declaration << "for " << "structure" << "\n";

  configuration_declaration << cli_configs;

  VHDLCompDeclListIter nextCompDecl(mainCompDeclList);
  VHDLCompDecl* compDecl;
  while ((compDecl = nextCompDecl++) != 0) {
    if (!(myTable.hasKey(compDecl->type))) {
      myTable.insert(compDecl->type, compDecl);

      // Filter out the CLI components C2V,V2C integer, real.
      if (!strcmp(compDecl->type,"C2Vreal")) continue;
      if (!strcmp(compDecl->type,"V2Creal")) continue;
      if (!strcmp(compDecl->type,"C2Vinteger")) continue;
      if (!strcmp(compDecl->type,"V2Cinteger")) continue;

      level++;
      configuration_declaration << indent(level) << "for all:"
				<< compDecl->type
				<< " use entity " << "work." << compDecl->type
				<< "(behavior); end for;\n";
      level--;
    }
  }

  configuration_declaration << "end " << "for" << ";\n";
  configuration_declaration << "end " << (const char*) filePrefix << "_parts"
			    << ";\n";
}

// Generate the clock generator entity and architecture.
StringList ArchTarget :: clockGenCode() {
  // This comes from CGTarget::mainLoopCode() since it isn't exported
  // out of there, but beginIteration is called from there.
  // We may need to change the in-wormhole behavior sometime.
  int repetitions = inWormHole()? -1 : (int)scheduler()->getStopTime();

  StringList codeList;
  codeList << "\n";
  codeList << "     -- ClockGen : clock generator\n";
  codeList << "entity ClockGen is\n";
  codeList << "     port (system_clock: out boolean;\n";
  codeList << "           iter_clock: in boolean;\n";
  codeList << "           iter_count: inout integer);\n";
  codeList << "end ClockGen;\n";
  codeList << "\n";
  codeList << "architecture behavior of ClockGen is\n";
  codeList << "begin\n";
  codeList << "     main: process\n";
  codeList << "     	variable internal: boolean := FALSE;\n";
  codeList << "     begin\n";
  codeList << "		if iter_count < " << repetitions << " then\n";
  codeList << "         	wait for 1 ps;\n";
  codeList << "         	internal := not internal;\n";
  codeList << "         	system_clock <= internal;\n";
  codeList << "     	else\n";
  codeList << "     		wait;\n";
  codeList << "     	end if;\n";
  codeList << "     end process main;\n";
  codeList << "\n";
  codeList << "     count: process\n";
  codeList << "     	variable internal: integer := 0;\n";
  codeList << "     begin\n";
  codeList << "		wait until iter_clock'event and iter_clock = TRUE;\n";
  codeList << "		internal := internal + 1;\n";
  codeList << "         iter_count <= internal;\n";
  codeList << "     end process count;\n";
  codeList << "end behavior;\n";
  codeList << "\n";
  return codeList;
}

// Generate the register entity and architecture.
StringList ArchTarget :: regCode(StringList type) {
  StringList codeList;
  codeList << "\n";
  codeList << "     -- Reg_" << type << " : register of type " << type << "\n";
  codeList << "entity Reg_" << type << " is\n";
  codeList << "     port (C: in boolean; D: in " << type << "; Q: out " << type << ");\n";
  codeList << "end Reg_" << type << ";\n";
  codeList << "\n";
  codeList << "architecture behavior of Reg_" << type << " is\n";
  codeList << "begin\n";
  codeList << "     main: process\n";
  codeList << "     begin\n";
  codeList << "          wait until C'event and C = TRUE;\n";
  codeList << "          Q <= D;\n";
  codeList << "     end process main;\n";
  codeList << "end behavior;\n";
  codeList << "\n";
  return codeList;
}

// Generate the multiplexor entity and architecture.
StringList ArchTarget :: muxCode(StringList type) {
  StringList codeList;
  codeList << "\n";
  codeList << "     -- Mux_" << type << " : initial value multiplexor of type " << type << "\n";
  codeList << "entity Mux_" << type << " is\n";
  codeList << "	port(\n";
  codeList << "		control: IN BOOLEAN;\n";
  codeList << "		init_val: IN " << type << ";\n";
  codeList << "		input: IN " << type << ";\n";
  codeList << "		output: OUT " << type << "\n";
  codeList << "	);\n";
  codeList << "end Mux_" << type << ";\n";
  codeList << "\n";
  codeList << "architecture behavior of Mux_" << type << " is\n";
  codeList << "	begin\n";
  codeList << "		process (control, init_val, input)\n";
  codeList << "		begin\n";
  codeList << "			if control then\n";
  codeList << "				output <= input;\n";
  codeList << "			else\n";
  codeList << "				output <= init_val;\n";
  codeList << "			end if;\n";
  codeList << "		end process;\n";
  codeList << "end behavior;\n";
  codeList << "\n";
  return codeList;
}

// Generate the source entity and architecture.
StringList ArchTarget :: sourceCode(StringList type) {
  StringList codeList;
  codeList << "\n";
  codeList << "     -- Source_" << type << " : constant generator\n";
  codeList << "entity Source_" << type << " is\n";
  codeList << "	generic(\n";
  codeList << "		value: " << type << "\n";
  codeList << "	);\n";
  codeList << "	port(\n";
  codeList << "		output: OUT " << type << "\n";
  codeList << "	);\n";
  codeList << "end Source_" << type << ";\n";
  codeList << "\n";
  codeList << "architecture behavior of Source_" << type << " is\n";
  codeList << "	begin\n";
  codeList << "		output <= value;\n";
  codeList << "end behavior;\n";
  codeList << "\n";
  return codeList;
}

// Generate a comment.
StringList ArchTarget :: comment(const char* text, const char* b,
				 const char* e, const char* c) {
    const char* begin = "-- ";
    const char* end = "";
    const char* cont = "-- ";
    if (b != NULL) begin = b;
    if (e != NULL) end = e;
    if (c != NULL) cont = c;
    return HLLTarget::comment(text, begin, end, cont);
}

// Generate code to begin an iterative procedure
void ArchTarget :: beginIteration(int /*repetitions*/, int /*depth*/) {
}

// Generate code to end an iterative procedure
void ArchTarget :: endIteration(int /*reps*/, int /*depth*/) {
  //  toggleClock("iter_clock");
}

// Pulse the given clock TRUE at the given time.
void ArchTarget :: pulseClock(const char* clockName, int clockTime) {
  clockList << clockName;
  ctlerAction << "     -- Pulse " << clockName << " TRUE\n";
  ctlerAction << "if clockCount = " << clockTime << " then\n";
  ctlerAction << "     " << clockName << " <= TRUE;\n";
  ctlerAction << "else\n";
  ctlerAction << "     " << clockName << " <= FALSE;\n";
  ctlerAction << "end if;\n";

  ctlerVariableList.put("clockCount", "INTEGER", "0");
  ctlerPortList.put(clockName, "boolean", "OUT", clockName, NULL);
  ctlerSignalList.put(clockName, "boolean");
}

// Add the clock to the clock list and generate code to toggle it.
void ArchTarget :: toggleClock(const char* clock) {
  // Pulse it HI-LO if it isn't already at the end of the list.
  if (!(clockList.tail()) || (strcmp(clockList.tail(),clock))) {
    clockList << clock;
    ctlerAction << "     -- Toggle " << clock << "\n";
    ctlerAction << "wait until "
		<< "system_clock'event and system_clock = TRUE;\n";
    ctlerAction << clock << " <= TRUE;\n";
    ctlerAction << "wait until "
		<< "system_clock'event and system_clock = TRUE;\n";
    ctlerAction << clock << " <= FALSE;\n";
    ctlerPortList.put(clock, "boolean", "OUT", clock, NULL);
    ctlerSignalList.put(clock, "boolean");
  }
}

// Assert the given clock with the given value at the given time.
void ArchTarget :: assertValue(const char* clockName, int clockTime,
			       int value) {
  clockList << clockName;
  ctlerAction << "     -- Assert " << clockName << " with " << value << " \n";
  ctlerAction << "if clockCount = " << clockTime << " then\n";
  ctlerAction << "     " << clockName << " <= " << value <<";\n";
  ctlerAction << "end if;\n";

  ctlerVariableList.put("clockCount", "INTEGER", "0");
  ctlerPortList.put(clockName, "INTEGER", "OUT", clockName, NULL);
  ctlerSignalList.put(clockName, "INTEGER");
}

// Add the clock to the clock list and generate code
// to assert it with the given value.
void ArchTarget :: assertClock(const char* clock, int value) {
  ctlerPortList.put(clock, "INTEGER", "OUT", clock, NULL);
  ctlerPortList.put("system_clock", "boolean", "IN", "system_clock", NULL);

  //  if (!(clockList.tail()) || (strcmp(clockList.tail(),clock))) {
    clockList << clock;
    ctlerAction << "     -- Assert " << clock << "\n";
    ctlerAction << "wait until "
		<< "system_clock'event and system_clock = TRUE;\n";
    ctlerAction << clock << " <= " << value << ";\n";
    ctlerPortList.put(clock, "INTEGER", "OUT", clock, NULL);
    ctlerSignalList.put(clock, "INTEGER");
    //  }
}

int ArchTarget :: codeGenInit() {
  // Set the names of all geodesics.
  setGeoNames(*galaxy());
  // Call initCode for each star.
  GalStarIter nextStar(*galaxy());
  VHDLStar* s;
  while ((s = (VHDLStar*) nextStar++) != 0) {
    // Nothing needs to be done for forks.
    if (s->isItFork()) continue;
    
    // Generate PortHole initialization code before the Star is run
    // because running the Star modifies the PortHole indices.
    s->initCode();
  }
  return TRUE;
}

// Method to write out com file for VSS if needed.
void ArchTarget :: writeComFile() {
  // Make sure to do the com file uniquely too!!!
  StringList comCode = "";
  comCode << "cd " << (const char*) filePrefix << "\n";
  comCode << "assign 0.0 *'vhdl\n";
  comCode << "run\n";
  comCode << simWrapup;
  comCode << "quit\n";
  writeFile(comCode, ".com", 0);
}

// Add additional codeStreams.
void ArchTarget :: addCodeStreams() {
  addStream("entity_declaration", &entity_declaration);
  addStream("architecture_body_opener", &architecture_body_opener);
  addStream("component_declarations", &component_declarations);
  addStream("mux_declarations", &mux_declarations);
  addStream("signal_declarations", &signal_declarations);
  addStream("component_mappings", &component_mappings);
  addStream("architecture_body_closer", &architecture_body_closer);
  addStream("configuration_declaration", &configuration_declaration);
  addStream("preSynch", &preSynch);
  addStream("firingAction", &firingAction);
  addStream("postSynch", &postSynch);
  addStream("ctlerAction", &ctlerAction);

  SimVSSTarget::addCodeStreams();
}

// Initialize codeStreams.
void ArchTarget :: initCodeStreams() {
  entity_declaration.initialize();
  architecture_body_opener.initialize();
  component_declarations.initialize();
  mux_declarations.initialize();
  signal_declarations.initialize();
  component_mappings.initialize();
  architecture_body_closer.initialize();
  configuration_declaration.initialize();
  preSynch.initialize();
  firingAction.initialize();
  postSynch.initialize();
  ctlerAction.initialize();

  SimVSSTarget::initCodeStreams();
}

// Initialize VHDLObjLists.
void ArchTarget :: initVHDLObjLists() {
  systemPortList.initialize();
  mainCompDeclList.initialize();
  sourceCompDeclList.initialize();
  muxCompDeclList.initialize();
  regCompDeclList.initialize();
  signalList.initialize();
  stateList.initialize();

  masterFiringList.initialize();
  newFiringList.initialize();
  firegroupList.initialize();
  dependencyList.initialize();
  iterDependencyList.initialize();
  tokenList.initialize();
  muxList.initialize();
  regList.initialize();
  clockList.initialize();

  ctlerGenericList.initialize();
  ctlerPortList.initialize();
  ctlerSignalList.initialize();
  ctlerVariableList.initialize();

  SimVSSTarget::initVHDLObjLists();
}

// Initialize firing lists.
void ArchTarget :: initFiringLists() {
  firingGenericList.initialize();
  firingPortList.initialize();
  firingSignalList.initialize();
  firingVariableList.initialize();
}
