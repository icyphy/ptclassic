defstar {
	name {Nop}
	domain {HOF}
	derivedfrom {Base}
	desc {
Bridge inputs to outputs and then self-destruct.
This star is used to split a bus into individual
lines or combine individual lines into a bus.
It is also used to break out multi-inputs
and multi-outputs into individual ports.
	}
	htmldoc {
Normally, the number of input connections must equal the number of
output connections.
But, a special feature of this star supports a particularly convenient
graphical programming device.
If either the input or the output is connected to a multi-porthole,
but too few connections have been realized in the multi-porthole,
then the star will increase the number of connections automatically
to the correct number.
	}
	version {$Id$}
	author { E. A. Lee and D. Niehaus }
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
	        name {input}
		type {anytype}
	}
	ccinclude {"Galaxy.h"}
	method {
	  name { preinitialize }
	  access { public }
	  code {
	    MPHIter nexti(input);
	    MPHIter nexto(output);
	    PortHole *pi = nexti++;
	    PortHole *po = nexto++;
	    PortHole *prevpifar = 0;
	    PortHole *prevpofar = 0;

	    while ((pi != 0) || (po != 0)) {
	      PortHole *source, *sink;
	      MultiPortHole *mph;
	      GenericPort *gpo, *gpi;
	      int numInDelays, numOutDelays;
	      const char *sourceDelayVals, *sinkDelayVals;

	      if (pi == 0) {
		// Out of inputs.
		// Try to create a new input connection.
		// Fail if we can't.
		if (!prevpifar) {
		  Error::abortRun(*this, "No inputs connected!");
		  return;
		}
		if ((mph = prevpifar->getMyMultiPortHole()) == 0) {
		  Error::abortRun(*this,"Not enough inputs for the given number of outputs");
		  return;
		}
		// Far side is a MultiPortHole -- can make a new connection
		source = &(mph->newConnection());
		if ((sink = prevpofar = po->far()) == 0) {
		  Error::abortRun(*this,"Can't find sink PortHole!");
		  return;
		}
		gpo = aliasPointingAt(po);
		gpi = 0;
		numInDelays = prevpifar->numInitDelays();
		numOutDelays = po->numInitDelays();
		sinkDelayVals = po->initDelayValues();
		sourceDelayVals = prevpifar->initDelayValues();
		sink->disconnect();
	      } else if (po == 0) {
		// Out of outputs.
		// Try to create a new output connection.
		// Fail if we can't.
		if (!prevpofar) {
		  Error::abortRun(*this, "No outputs connected!");
		  return;
		}
		if ((mph = prevpofar->getMyMultiPortHole()) == 0) {
		  Error::abortRun(*this,"Not enough outputs for the given number of inputs");
		  return;
		}
		// Far side is a MultiPortHole -- can make a new connection
		sink = &(mph->newConnection());
		if ((source = prevpifar = pi->far()) == 0) {
		  Error::abortRun(*this,"Can't find source PortHole!");
		  return;
		}
		gpi = aliasPointingAt(pi);
		gpo = 0;
		numInDelays = pi->numInitDelays();
		numOutDelays = prevpofar->numInitDelays();
		sourceDelayVals = pi->initDelayValues();
		sinkDelayVals = prevpofar->initDelayValues();
		source->disconnect();
	      } else {
		if ((source = pi->far()) == 0 || (sink = po->far()) == 0) {
		  Error::abortRun(*this,"Star is not fully connected");
		  return;
		}
		gpo = aliasPointingAt(po);
		gpi = aliasPointingAt(pi);
		numInDelays = pi->numInitDelays();
		numOutDelays = po->numInitDelays();
		sourceDelayVals = pi->initDelayValues();
		sinkDelayVals = po->initDelayValues();
		prevpofar = po->far();
		prevpifar = pi->far();
		source->disconnect();
		sink->disconnect();
	      }

	      int numDelays = 0;
	      const char* delayVals = 0;
	      if ((numInDelays > 0) || (sourceDelayVals && *sourceDelayVals)) {
		numDelays = numInDelays;
		delayVals = sourceDelayVals;
		if ((numOutDelays > 0) || (sinkDelayVals && *sinkDelayVals)) {
		  Error::warn(*this,
			      "Cannot have delays on both inputs and outputs."
			      " Using input value");
		}
	      } else {
		numDelays = numOutDelays;
		delayVals = sinkDelayVals;
	      }
	      connectPorts(*source,*sink,numDelays,delayVals);
	      fixAliases(gpi,pi,sink);
	      fixAliases(gpo,po,source);

	      po = nexto++;
	      pi = nexti++;
	    }
	    // Now remove ourselves from the parent galaxy and self-destruct
	    Galaxy* mom = idParent();
	    if(!mom) return;
	    mom->deleteBlockAfterInit(*this);
	  }
	}
}
