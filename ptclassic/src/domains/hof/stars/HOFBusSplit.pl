defstar {
	name {BusSplit}
	domain {HOF}
	derivedfrom {Base}
	desc {
Bridge inputs to outputs and then self-destruct.
This star splits an input bus into two.
If the input bus width is N, and the output bus widths
are M1 and M2, then we require that N = M1 + M2.
The first M1 inputs go the first output bus, while
the next M2 inputs go to the second output bus.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { HOF main library }
	outmulti {
		name {top}
		type {anytype}
	}
	outmulti {
		name {bottom}
		type {anytype}
	}
	inmulti {
	        name {input}
		type {anytype}
	}
	ccinclude {"Galaxy.h"}

	method {
	  name { preinitialize }
	  access { public }
	  code {
	    HOFBase::preinitialize();

	    // Make sure we know the number of connections on the
	    // input and output multiPortHoles.
	    initConnections(top);
	    initConnections(bottom);
	    initConnections(input);

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
	      connectPorts(*source,*sink,numDelays,delayVals);
	      fixAliases(gpi,pi,sink);
	      fixAliases(gpo,po,source);
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
}
