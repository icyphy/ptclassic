defstar {
	name {BusInterleave}
	domain {HOF}
	derivedfrom {Base}
	desc {
Bridge inputs to outputs and then self-destruct.
This star interleaves two input busses onto a single bus.
The two input busses must have the same width, which must
be half the width of the output bus.
The input signals are connected to the output in an alternating fashion.
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
		name {output}
		type {anytype}
	}
	inmulti {
	        name {top}
		type {anytype}
	}
	inmulti {
	        name {bottom}
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
	    initConnections(output);
	    initConnections(top);
	    initConnections(bottom);

	    MPHIter nexttop(top);
	    MPHIter nextbottom(bottom);
	    // start with the top inputs
	    MPHIter *nexti = &nexttop;
	    MPHIter *previ = &nextbottom;
	    MPHIter nexto(output);
	    PortHole *source, *sink;
	    const char *sourceDelayVals, *sinkDelayVals;
	    int numInDelays, numOutDelays;

	    PortHole *pi, *po;
	    while ((po = nexto++) != 0) {
	      if ((pi = (*nexti)++) == 0) {
		// Out of inputs, flag error
		Error::abortRun(*this,
				"Not enough inputs for the given number of outputs");
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
	      MPHIter *tmp = previ;
	      previ = nexti;
	      nexti = tmp;
	    }                        // while loop
	    // Out of outputs at this point.
	    // Check to be sure we are also out of inputs.
	    if ((nextbottom++ != 0) || (nexttop++ != 0)) {
	      Error::abortRun(*this,
			      "Not enough outputs for the given number of inputs");
	      return;
	    }
	    // Now remove ourselves from the parent galaxy and self-destruct
	    Galaxy* mom = idParent();
	    if(!mom) return;
	    mom->deleteBlockAfterInit(*this);
	  }
	}
}

