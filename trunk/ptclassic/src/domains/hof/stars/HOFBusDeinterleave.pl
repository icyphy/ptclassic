defstar {
	name {BusDeinterleave}
	domain {HOF}
	derivedfrom {Base}
	desc {
Bridge inputs to outputs and then self-destruct.
This star deinterleaves a bus, producing two output busses of equal width.
The input bus must have even width.
The even numbered input signals are connected to the
first output bus, while the odd numbered input signals
are connected to the second output bus.
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
	    MPHIter *prevo = &nextbottom;
	    MPHIter nexti(input);
	    PortHole *source, *sink;
	    const char *sourceDelayVals, *sinkDelayVals;
	    int numInDelays, numOutDelays;

	    PortHole *pi, *po;
	    while ((pi = nexti++) != 0) {
	      if ((po = (*nexto)++) == 0) {
		// Out of outputs, flag error
		Error::abortRun(*this,
				"Not enough outputs for the given number of inputs");
		return;
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

	      // Swap iterators
	      MPHIter *tmp = prevo;
	      prevo = nexto;
	      nexto = tmp;
	    }                        // while loop
	    // Out of inputs at this point.
	    // Check to be sure we are also out of outputs.
	    if ((nextbottom++ != 0) || (nexttop++ != 0)) {
	      Error::abortRun(*this,
			      "Not enough inputs for the given number of outputs");
	      return;
	    }
	    // Now remove ourselves from the parent galaxy and self-destruct
	    Galaxy* mom = idParent();
	    if(!mom) return;
	    mom->deleteBlockAfterInit(*this);
	  }
	}
}