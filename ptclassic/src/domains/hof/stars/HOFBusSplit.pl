defstar {
	name {BusSplit}
	domain {HOF}
	derivedfrom {Base}
	desc {
Bridge inputs to outputs and then self-destruct.
This star is used to split and input bus into two.
	}
	explanation {
The total number of output connections (the sum of the
two output bus widths) must equal the number of input connections.
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
		name {top}
		type {=input}
	}
	outmulti {
		name {bottom}
		type {=input}
	}
	inmulti {
	        name {input}
		type {anytype}
	}
	ccinclude {"Galaxy.h"}
	ccinclude {"InterpGalaxy.h"}
	setup {

	  MPHIter nexttop(top);
	  MPHIter nextbottom(bottom);
	  // start with the top inputs
	  MPHIter *nexto = &nexttop;
	  MPHIter nexti(input);
	  PortHole *source, *sink;
	  const char *sourceDelayVals, *sinkDelayVals;
	  int numInDelays, numOutDelays;

	  PortHole *pi, *po;
	  while ((pi = nexti++) != 0) {
	    if ((po = (*nexto)++) == 0) {
	      // Out of outputs, switch to the bottom outputs.
	      nexto = &nextbottom;
	      if ((po = (*nexto)++) == 0) {
		// Still out of outputs
		Error::abortRun(*this,"Not enough outputs for the given number of inputs");
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

	    // Get alias pointers before disconnecting
	    GenericPort *gpo = aliasPointingAt(po);
	    GenericPort *gpi = aliasPointingAt(pi);

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
	    fixAliases(gpi,pi,sink);
	    fixAliases(gpo,po,source);

	    source->initialize();
	    sink->initialize();
	  }                        // while loop
	  // Out of inputs at this point.
	  // Check to be sure we are also out of outputs.
	  if ((nextbottom++ != 0) || (nexttop++ != 0)) {
	    Error::abortRun(*this,"Not enough inputs for the given number of outputs");
	    return;
	  }
	  // Now remove ourselves from the parent galaxy and self-destruct
	  Galaxy* mom = idParent();
	  if(!mom) return;
	  mom->deleteBlockAfterInit(*this);
	}
}

