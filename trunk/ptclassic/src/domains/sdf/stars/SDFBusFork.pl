defstar {
	name {BusFork}
	domain {SDF}
	desc { Copy particles from an input bus to each output bus. }
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
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
	  MPHIter nextin(input), nextoutA(outputA), nextoutB(outputB);
	  PortHole *in, *outA = (PortHole *)NULL, *outB = (PortHole *)NULL;
	  while (((in = nextin++) != 0) &&
		 ((outA = nextoutA++) != 0) &&
		 ((outB = nextoutB++) != 0)) {
	    (*outA)%0 = (*outB)%0 = (*in)%0;
	  }
	}
}
