defstar {
name { PeekPoke }
domain { AnyCG }
desc { Nondeterminate communication link, splices in a peek/poke pair }
version { $Id$ }
author { Jose Luis Pino }
copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
}
location { CG Star Directory }
ccinclude { "Galaxy.h","EventHorizon.h","MultiTarget.h"  }
	htmldoc {
This star allows for peek/poke communications as was described in, currently
this is only supported in multitargets.
<p>
J.L. Pino, T.M. Parks and E.A. Lee, "Mapping Multiple Independent Synchronous
Dataflow Graphs onto Heterogeneous Multiprocessors," <i>Proc. of IEEE Asilomar
Conf. on Signals, Systems, and Computers</i>, Pacific Grove, CA,
Oct. 31 - Nov. 2, 1994.
&lt;http://ptolemy.eecs.berkeley.edu/papers/hierStaticSched-asilomar-95&gt;
<p>
The peek/poke facilities implement only a limited part of the
functionality described in the Asilomar paper.
<p>
The first two user settable states are the <i>Poke_procId</i> and
<i>Peek_procId</i>.  These must be set to the processor id where the poke
actor and peek actor are to be mapped to.
<p>
The next user settable state is <i>blockSize</i>.  With this state a
peek/poke link can transfer <i>blockSize</i> number of consequtive
samples.
<p>
The final user settable state is <i>updateRate</i>.  This state can be
considered as a nondeterminate downsample factor for the link.
The link will be updated at most once every <i>updateRate</i> number of
iterations of one of the two disconnected SDF graphs.  The side which
this effects is determined by the specific peek/poke implementation.
}

ccinclude { "miscFuncs.h" }
input {
    name {input}
    type {ANYTYPE}
}

output {
    name {output}
    type {=input}
}

state {
    name { Poke_procId }
    type { int }
    default { 1 }
    desc { assigned processor id for poke - must be manually assigned. }
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
	if (!farPort) {
	    Error::abortRun(*this,startPort.name()," not connected");
	} else {
	  // Remove all galaxy references to the startPort
	  removeGalaxyPort(startPort.aliasFrom());
	  // Remove the all galaxy references to the farPort
	  removeGalaxyPort(farPort->aliasFrom());
	}
	return farPort;
    }
}

constructor {
    procId.setState("Peek_procId",this, "0",
		    "assigned processor id for peek - must be manually "
		    "assigned.");
}

method {
    name { findMultiTarget }
    arglist { "(Target* multitarget)" }
    type { "inline Target*" }
    code
    {
	while (multitarget && !multitarget->isA("MultiTarget"))
	    multitarget = (Target*) multitarget->parent();
	if (multitarget) return multitarget;
	return (Target*)NULL;
    }
}

setup {
    // Try to find the multitarget by going up the target parent
    Target* multitarget = findMultiTarget(target());

    // Now if we haven't found the multitarget yet - we iterate over
    // the parent wormholes
    Block* prnt = this;
    while ((prnt = prnt->parent()) != NULL) {
	// If my parent is not atomic, it is not a wormhole
	if (!prnt->isItAtomic()) continue;

	// My parent is atomic, it is a wormhole - check its
	// target to see if it is a multitarget
	multitarget = findMultiTarget(prnt->target());
	if (multitarget) break;
    }
    
    if (!multitarget) {
	Error::abortRun(*this,"is not inside of a multitarget");
	return;
    }

    if (multitarget->nProcs() <= (int)procId || procId < 0 ) {
	StringList message;
	message << "The Peek_ProcId is invalid, it is set to "
		<< procId << " but the number of child targets is "
		<< multitarget->nProcs();
	Error::abortRun(*this,message);
	return;
    }

    if (multitarget->nProcs() <= Poke_procId || Poke_procId < 0) {
	StringList message;
	message << "The Poke_ProcId is invalid, it is set to "
		<< Poke_procId << " but the number of child targets is "
		<< multitarget->nProcs();
	Error::abortRun(*this,message);
	return;
    }

    PortHole* peekPort = findPort(output);
    PortHole* pokePort = findPort(input);
    if (!(peekPort && pokePort)) return;

    AsynchCommPair 
	pair = ((MultiTarget*)multitarget)
	    ->createPeekPokeProcId(procId,Poke_procId);

    CGStar *peek = pair.peek, *poke = pair.poke;

    if (peek == NULL || poke == NULL) return;

    PortHole* input = poke->portWithName("input");
    if (!input) {
	Error::abortRun(*this,poke->name(),
			": Poke star does not have a porthole named 'input'");
	return;
    }	

    PortHole* output = peek->portWithName("output");
    if (!output) {
	Error::abortRun(*this,peek->name(),
			": Peek star does not have a porthole named 'output'");
	return;
    }

    if (output->type() == ANYTYPE) input->inheritTypeFrom(*output);

    int numInitDelays;
    Galaxy* parentGal;
    StringList starName, linkName;
    linkName << starSymbol.lookup("Link");
    // FIXME - we do not support initial delays on peek/poke arcs
    // The peekPort and pokePort are in a disconnected state here.
    // numInitDelays = peekPort->numInitDelays();
    numInitDelays = 0;
    peekPort->disconnect();
    output->connect(*peekPort,numInitDelays);
    parentGal = (Galaxy*) peekPort->parent()->parent();
    starName.initialize();
    starName << "Peek_" << linkName ;
    parentGal->addBlock(*peek,hashstring(starName));
    peek->setTarget(multitarget->child(procId));

    // FIXME - we do not support initial delays on peek/poke arcs
    // The peekPort and pokePort are in a disconnected state here.
    // numInitDelays = pokePort->numInitDelays();
    numInitDelays = 0;
    pokePort->disconnect();
    pokePort->connect(*input,numInitDelays);
    parentGal = (Galaxy*) pokePort->parent()->parent();
    starName.initialize();
    starName << "Poke_" << linkName;
    parentGal->addBlock(*poke,hashstring(starName));
    poke->setTarget(multitarget->child(Poke_procId));

    if ((int) blockSize > 1) 
	setAsynchCommState(pair,"blockSize",blockSize.currentValue());
    if ((int) updateRate > 1)
	setAsynchCommState(pair,"updateRate",updateRate.currentValue());
    ((Galaxy*) parent())->deleteBlockAfterInit(*this);

    peek->setProcId(procId);
    poke->setProcId(Poke_procId);
    
}

}

