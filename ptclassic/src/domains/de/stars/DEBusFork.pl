defstar {
	name {BusFork}
	domain {DE}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	desc { Replicate events from an input bus to each output bus with zero delay. }
	inmulti {
		name{input}
		type{ANYTYPE}
	}
	outmulti {
	  name{outputA}
	  type{= input}
	}
	outmulti {
	  name{outputB}
	  type{= input}
	}
	// The following test is done in the begin method rather than
	// setup so that inputs or outputs can have Nop stars connected.
	begin {
	  if (input.numberPorts() != outputA.numberPorts() ||
	      input.numberPorts() != outputB.numberPorts()) {
	    Error::abortRun(*this, "Bus widths don't match");
	    return;
	  }
	}
	go {
	  completionTime = arrivalTime;
	  InDEMPHIter nextin(input);
	  OutDEMPHIter nextoutA(outputA), nextoutB(outputB);
	  InDEPort *in;
	  OutDEPort *outA, *outB;
	  while (((in = nextin++) != 0) &&
		 ((outA = nextoutA++) != 0) &&
		 ((outB = nextoutB++) != 0)) {
	    if (in->dataNew) {
	      Particle& pp = in->get();
	      outA->put(completionTime) = pp;
	      outB->put(completionTime) = pp;
	    }
	  }
	}
}
