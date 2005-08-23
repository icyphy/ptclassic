defstar {
	name {Nop}
	domain {HOF}
	derivedfrom {Base}
	version { @(#)HOFNop.pl	1.11 11/13/97 }
	author { Edward A. Lee, Tom Lane }
	location { HOF main library }
	copyright {
Copyright (c) 1994-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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

	inmulti {
	        name {input}
		type {anytype}
	}
	outmulti {
		name {output}
		type {anytype}
	}

	method {
	  name { doExpansion }
	  type { int }
	  code {
	    // Note I must NOT call initConnections.

	    // We need to keep track of whether a unique far multiporthole
	    // has been seen on the input and output sides.  If there is
	    // more than one, we can't reasonably pick one to expand.
	    MultiPortHole *inMPH = 0;
	    MultiPortHole *outMPH = 0;
	    int uniqueInMPH = 0;
	    int uniqueOutMPH = 0;
	    // When we do add a connection, we want to duplicate the delays
	    // found on the last pre-existing connection to that MPH.
	    int numInDelaysMPH = 0;
	    int numOutDelaysMPH = 0;
	    const char *sourceDelayValsMPH = 0;
	    const char *sinkDelayValsMPH = 0;

	    // If the Nop's own input or output MPH has a galaxy MPH aliased
	    // to it, then for each additional port we create, we must also
	    // make another alias port.  Otherwise a HOFNop just inside a
	    // galaxy boundary doesn't work the same as a genuine bus through
	    // the boundary; all the bus members must have aliases.
	    MultiPortHole *inputMPHAlias = 0;
	    if (input.aliasFrom())
	      inputMPHAlias = (MultiPortHole*) &(input.getTopAlias());
	    MultiPortHole *outputMPHAlias = 0;
	    if (output.aliasFrom())
	      outputMPHAlias = (MultiPortHole*) &(output.getTopAlias());

	    MPHIter nexti(input);
	    MPHIter nexto(output);
	    PortHole *pi = nexti++;
	    PortHole *po = nexto++;
	    while ((pi != 0) || (po != 0)) {
	      int numInDelays, numOutDelays;
	      const char *sourceDelayVals, *sinkDelayVals;
	      MultiPortHole *mph;

	      // Get the far source porthole, or make a new one.
	      PortHole *source;
	      if (pi != 0) {
		if ((source = pi->far()) == 0) {
		  Error::abortRun(*pi,"Star is not fully connected");
		  return 0;
		}
		numInDelays = pi->numInitDelays();
		sourceDelayVals = pi->initDelayValues();
		if ((mph = source->getMyMultiPortHole()) != 0) {
		  mph = (MultiPortHole*) &(mph->getTopAlias());
		  if (inMPH == 0) {
		    inMPH = mph;
		    uniqueInMPH = 1;
		  } else if (inMPH != mph) {
		    uniqueInMPH = 0;
		  }
		  numInDelaysMPH = numInDelays;
		  sourceDelayValsMPH = sourceDelayVals;
		}
		source->disconnect();
	      } else {
		// Out of inputs.
		// Try to create a new input connection.
		// Fail if we can't.
		if (!uniqueInMPH) {
		  Error::abortRun(*this,
				  "Not enough inputs for the given number of outputs");
		  return 0;
		}
		// Far side is a MultiPortHole -- can make a new connection
		source = &(inMPH->newConnection());
		numInDelays = numInDelaysMPH;
		sourceDelayVals = sourceDelayValsMPH;
		// Make another galaxy alias port if needed.
		if (inputMPHAlias) {
		  // We force the alias to create a new port even though
		  // there are probably disconnected members of its bottom
		  // alias (namely, Nop's input MPH).
		  pi = &(inputMPHAlias->newPort().newConnection());
		  // The input MPH now contains a new member (namely, *pi).
		  // Will the iterator nexti pick it up on the next call?
		  // Just to be sure it won't, we advance nexti.
		  nexti++;
		}
	      }

	      // Get the far sink porthole, or make a new one.
	      PortHole *sink;
	      if (po != 0) {
		if ((sink = po->far()) == 0) {
		  Error::abortRun(*po,"Star is not fully connected");
		  return 0;
		}
		numOutDelays = po->numInitDelays();
		sinkDelayVals = po->initDelayValues();
		if ((mph = sink->getMyMultiPortHole()) != 0) {
		  mph = (MultiPortHole*) &(mph->getTopAlias());
		  if (outMPH == 0) {
		    outMPH = mph;
		    uniqueOutMPH = 1;
		  } else if (outMPH != mph) {
		    uniqueOutMPH = 0;
		  }
		  numOutDelaysMPH = numOutDelays;
		  sinkDelayValsMPH = sinkDelayVals;
		}
		sink->disconnect();
	      } else {
		// Out of outputs.
		// Try to create a new output connection.
		// Fail if we can't.
		if (!uniqueOutMPH) {
		  Error::abortRun(*this,
				  "Not enough outputs for the given number of inputs");
		  return 0;
		}
		// Far side is a MultiPortHole -- can make a new connection
		sink = &(outMPH->newConnection());
		numOutDelays = numOutDelaysMPH;
		sinkDelayVals = sinkDelayValsMPH;
		// Make another galaxy alias port if needed.
		if (outputMPHAlias) {
		  // We force the alias to create a new port even though
		  // there are probably disconnected members of its bottom
		  // alias (namely, Nop's output MPH).
		  po = &(outputMPHAlias->newPort().newConnection());
		  // The output MPH now contains a new member (namely, *po).
		  // Will the iterator nexto pick it up on the next call?
		  // Just to be sure it won't, we advance nexto.
		  nexto++;
		}
	      }

	      // This code should match HOFBase::crossConnect.
	      // Unfortunately we can't just use that routine,
	      // unless we want to make its interface far more complex.
	      int numDelays = 0;
	      const char* delayVals = 0;
	      if (numInDelays > 0 || (sourceDelayVals && *sourceDelayVals)) {
		numDelays = numInDelays;
		delayVals = sourceDelayVals;
		if (numOutDelays > 0 || (sinkDelayVals && *sinkDelayVals)) {
		  Error::warn(*this,
			      "Cannot have delays on both input and output."
			      " Using input value");
		}
	      } else {
		numDelays = numOutDelays;
		delayVals = sinkDelayVals;
	      }
	      connectPorts(*source,*sink,numDelays,delayVals,
			   po, pi);

	      pi = nexti++;
	      po = nexto++;
	    }

	    return 1;
	  }
	}
}
