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
	explanation {
Normally, the number of input connections must equal the number of
output connections.
But, a special feature of this star supports a particularly convenient
graphical programming device.
If either the input or the output is connected to a multi-porthole on
the far side, but there too few connections that have been realized,
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
		type {=input}
	}
	inmulti {
	        name {input}
		type {anytype}
	}
	ccinclude {"Galaxy.h"}
	ccinclude {"InterpGalaxy.h"}
	private {
	  int initFlag;
	}
	constructor {
	  initFlag = 0;
	}
	setup {
	  if (!initFlag) reconnect();
	  // Now remove ourselves from the parent galaxy and self-destruct
	  Galaxy* mom = idParent();
	  if(!mom) return;
	  mom->deleteBlockAfterInit(*this);
	}
	// A public method to accomplish the reconnection is made available
	// so that other HOF stars that depend on knowing the number of inputs
	// and output can force the reconnection to occur without actually
	// initializing the star (which would cause it to self-destruct).
	// This is a violation of information hiding, but I see no way around
	// it.
	method {
	  name { reconnect }
	  type { "void" }
	  access { public }
	  arglist { "()" }
	  code {
	    MPHIter nexti(input);
	    MPHIter nexto(output);
	    PortHole *source, *sink;
	    MultiPortHole *mph;
	    const char *sourceDelayVals, *sinkDelayVals;
	    int numInDelays, numOutDelays;

	    if (!initFlag) {
	      initFlag = 1;

	      // A NamedObjList is used to collect pointers to the Blocks at
	      // the other end of modified connections.  After completing all the
	      // reconnections, we go through and initialize all these objects.
	      // The reason that the blocks need to be reinitialized, and not just
	      // their portholes, is that the number of connections to the block
	      // may have changed.

	      NamedObjList objectsToReinitialize;
	      PortHole *prevpofar = 0;
	      PortHole *prevpifar = 0;
	      PortHole *pi = nexti++;
	      PortHole *po = nexto++;
	      GenericPort *gpo, *gpi;
	      while ((pi != 0) ||
		     (po != 0)) {
		if (pi == 0) {
		  // Out of inputs.
		  // Try to create a new input connection.
		  // Fail if we can't.
		  if (!prevpifar) {
		    Error::abortRun(*this, "No inputs connected!");
		    return;
		  }
		  if ((mph = prevpifar->getMyMultiPortHole()) != 0) {
		    // Far side is a MultiPortHole -- can make a new connection
		    source = &(mph->newConnection());
		  } else {
		    Error::abortRun(*this,"Not enough inputs for the given number of outputs");
		    return;
		  }
		  if ((sink = prevpofar = po->far()) == 0) {
		    Error::abortRun(*this,"Can't find sink PortHole!");
		    return;
		  }
		  numInDelays = prevpifar->numInitDelays();
		  numOutDelays = po->numInitDelays();
		  sinkDelayVals = po->initDelayValues();
		  sourceDelayVals = prevpifar->initDelayValues();
		  gpo = aliasPointingAt(po);
		  gpi = 0;
		  sink->disconnect();
		} else if (po == 0) {
		  // Out of outputs.
		  // Try to create a new output connection
		  // Fail if we can't.
		  if (!prevpofar) {
		    Error::abortRun(*this, "No outputs connected!");
		    return;
		  }
		  if ((mph = prevpofar->getMyMultiPortHole()) != 0) {
		    // Far side is a MultiPortHole -- can make a new connection
		    sink = &(mph->newConnection());
		  } else {
		    Error::abortRun(*this,"Not enough outputs for the given number of inputs");
		    return;
		  }
		  if ((source = prevpifar = pi->far()) == 0) {
		    Error::abortRun(*this,"Can't find source PortHole!");
		    return;
		  }
		  numInDelays = pi->numInitDelays();
		  numOutDelays = prevpofar->numInitDelays();
		  sourceDelayVals = pi->initDelayValues();
		  sinkDelayVals = prevpofar->initDelayValues();
		  gpi = aliasPointingAt(pi);
		  gpo = 0;
		  source->disconnect();
		} else {
		  if((source = pi->far()) == 0 ||
		     (sink = po->far()) == 0) {
		    Error::abortRun(*this,"Star is not fully connected");
		    return;
		  }
		  sourceDelayVals = pi->initDelayValues();
		  sinkDelayVals = po->initDelayValues();
		  prevpofar = po->far();
		  prevpifar = pi->far();
		  numInDelays = pi->numInitDelays();
		  numOutDelays = po->numInitDelays();
		  gpo = aliasPointingAt(po);
		  gpi = aliasPointingAt(pi);
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

		// If the farside porthole belongs to a HOFStar, we can assume it
		// has not been initialized (since it would have self-destructed).
		// It should not be initialized here either, since it may be already
		// on the list of stars to be initialized, and since it self-
		// destructs when it initializes.

		// The actual initialization is postponed until after this star
		// has completed, in case they have behavior that depends on
		// the number of ports.

		NamedObj *farObj;
		if (!source->parent()->isA("HOFStar")) {
		  farObj = source->parent();
		  objectsToReinitialize.remove(farObj);
		  objectsToReinitialize.put(*farObj);
		}
		if (!sink->parent()->isA("HOFStar")) {
		  farObj = sink->parent();
		  objectsToReinitialize.remove(farObj);
		  objectsToReinitialize.put(*farObj);
		}
		po = nexto++;
		pi = nexti++;
	      }
	      objectsToReinitialize.initElements();
	    }
 	  }
	}
}

