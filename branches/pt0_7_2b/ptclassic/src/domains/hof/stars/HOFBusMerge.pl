defstar {
	name {BusMerge}
	domain {HOF}
	derivedfrom {BaseHiOrdFn}
	desc {
Bridge inputs to outputs and then self-destruct.
This star is used to merge two input busses into a single bus.
	}
	explanation {
The total number of input connections (the sum of the
two input bus widths) must equal the number of output connections.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { HOF main library }
	outmulti {
		name {output}
		type {=top}
	}
	inmulti {
	        name {top}
		type {anytype}
	}
	inmulti {
	        name {bottom}
		type {=top}
	}
	ccinclude {"Galaxy.h"}
	ccinclude {"InterpGalaxy.h"}
	setup {

	  MPHIter nexttop(top);
	  MPHIter nextbottom(bottom);
	  // start with the top inputs
	  MPHIter *nexti = &nexttop;
	  MPHIter nexto(output);
	  PortHole *source, *sink;
	  const char *sourceDelayVals, *sinkDelayVals;
	  int numInDelays, numOutDelays;

	  PortHole *pi, *po;
	  while ((po = nexto++) != 0) {
	    if ((pi = (*nexti)++) == 0) {
	      // Out of inputs, switch to the bottom inputs.
	      nexti = &nextbottom;
	      if ((pi = (*nexti)++) == 0) {
		// Still out of inputs
		Error::abortRun(*this,"Not enough inputs for the given number of outputs");
		return;
	      }
	    }
	    if((source = pi->far()) == 0 ||
	       (sink = po->far()) == 0) {
	      Error::abortRun(*this,"Star is not fully connected");
	      return;
	    }
	    sourceDelayVals = pi->initDelayValues();
	    sinkDelayVals = po->initDelayValues();
	    numInDelays = pi->numInitDelays();
	    numOutDelays = po->numInitDelays();
	    source->disconnect();
	    sink->disconnect();

	    int numDelays = 0;
	    const char* delayVals = 0;
	    if ((numInDelays > 0) || (sourceDelayVals && *sourceDelayVals)) {
	      numDelays = numInDelays;
	      delayVals = sourceDelayVals;
	      if ((numOutDelays > 0) || (sinkDelayVals && *sinkDelayVals)) {
		Error::warn(*this,
			    "Cannot have delays on inputs and outputs."
			    " Using input value");
	      }
	    } else {
	      numDelays = numOutDelays;
	      delayVals = sinkDelayVals;
	    }
	    source->connect(*sink,numDelays,delayVals);
	    // Register the connection with the parent galaxy so that delays get
	    // initialized when the galaxy is reinitialized.
	    if(parent()->isA("InterpGalaxy") && 
	       ((numDelays > 0) || (delayVals && *delayVals))) {
	      ((InterpGalaxy*)parent())->registerInit("C",
						      source->parent()->name(),
						      source->name(),
						      delayVals,
						      sink->parent()->name(),
						      sink->name());
	    }
	    // Fix aliases
	    GenericPort *gp;
	    if (pi) {
	      gp = pi->aliasFrom();
	      if(gp) gp->setAlias(*sink);
	    }
	    if (po) {
	      gp = po->aliasFrom();
	      if(gp) gp->setAlias(*source);
	    }

	    source->initialize();
	    sink->initialize();
	  }                        // while loop
	  // Out of outputs at this point.
	  // Check to be sure we are also out of inputs.
	  if ((nextbottom++ != 0) || (nexttop++ != 0)) {
	    Error::abortRun(*this,"Not enough outputs for the given number of inputs");
	    return;
	  }
	  // Now remove ourselves from the parent galaxy and self-destruct
	  Galaxy* mom = idParent();
	  if(!mom) return;
	  mom->deleteBlockAfterInit(*this);
	}
}
