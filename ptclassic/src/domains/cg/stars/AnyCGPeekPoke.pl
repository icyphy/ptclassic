defstar {
name { PeekPoke }
domain { AnyCG }
desc { Asynchronous communication star, splices in a peek/poke pair }
version { $Id$ }
author { Jose Luis Pino }
copyright {
Copyright (c) 1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
location { CG Star Directory }
ccinclude { "Galaxy.h","EventHorizon.h","MultiTarget.h"  }
explanation {
This star allows for peek/poke communications as was described in, currently
this is only supported in multitargets.

J.L. Pino, T.M. Parks and E.A. Lee, "Mapping Multiple Independent Synchronous
Dataflow Graphs onto Heterogeneous Multiprocessors," Proc. of IEEE Asilomar
Conf. on Signals, Systems, and Computers, Pacific Grove, CA, Oct. 31 - Nov. 2,
1994.
}

ccinclude { "miscFuncs.h" }
input {
    name {input}
    type {ANYTYPE}
}

output {
    name {output}
    type {ANYTYPE}
}

state {
    name { blockSize }
    type { int }
    default { 1 }
    desc { number of tokens to be transfered per firing. }
}

state {
    name { updateRate }
    type { int }
    default { 1 }
    desc {Update the link at most every 'updateRate' schedule iterations.}
}

method {
    name { removeGalaxyPort }
    type { "void"}
    access { private }
    arglist { "(GenericPort* port)" }
    code {
	if (port == NULL) return;
	if (port->aliasFrom()) removeGalaxyPort(port->aliasFrom());
	port->clearAliases();
	port->parent()->removePort(port->newPort());
	LOG_DEL; delete port;
    }
}

method {
    name { findPort }
    type { "PortHole*" }
    access { private }
    arglist {"(PortHole& startPort)"}
    code {
	PortHole* farPort = startPort.far();
	while (farPort && farPort->asEH()) {
	    // Find the inside port
	    PortHole* temp = farPort->asEH()->ghostAsPort()->far();

	    // Now remove the port from the wormhole
	    farPort->parent()->removePort(*farPort);
	    LOG_DEL; delete farPort;

	    // Now remove the corresponding ports from the galaxies inside 
	    // the wormhole
	    removeGalaxyPort(temp->aliasFrom());

	    farPort = temp;
	}
	if (!farPort)
	    Error::abortRun(*this,startPort.name()," not connected");

	// Remove all galaxy references to the startPort
	removeGalaxyPort(startPort.aliasFrom());
	// Remove the all galaxy references to the farPort
	removeGalaxyPort(farPort->aliasFrom());

	return farPort;
    }
}

method {
    name { setState }
    type { int }
    access { private }
    arglist {"(CGStar& peek, CGStar& poke, State& state)"}
    code {
	State* peekState = peek.stateWithName(state.name());
	State* pokeState = poke.stateWithName(state.name());
	if (!(peekState && pokeState)) {
	    Error::warn (*this,
	      "Sorry, the peek/poke actors do not support ",state.name());
	    return FALSE;
	}
	
	StringList currentValue;
	currentValue << state.currentValue();	
	const char* value = hashstring(currentValue);
	peekState->setInitValue(value);
	pokeState->setInitValue(value);
	peekState->initialize();
	pokeState->initialize();
	return TRUE;
    }
}
	
setup {
    Target* target = cgTarget();
    while (target && ! target->isA("MultiTarget"))
	target = (Target*) target->parent();
    if (!target) {
	Error::abortRun(*this,"is not inside of a multitarget");
	return;
    }
    PortHole* peekPort = findPort(output);
    PortHole* pokePort = findPort(input);
    if (!(peekPort && pokePort)) return;
    CGStar *peek, *poke;
    int status = 
	((MultiTarget*)target)->createPeekPoke(*peekPort,*pokePort,peek,poke);
    if (!status) return;
    if ((int) blockSize > 1) setState(*peek,*poke,blockSize);
    if ((int) updateRate > 1) setState(*peek,*poke,blockSize);
    ((Galaxy*) parent())->deleteBlockAfterInit(*this);
}

}

