defstar {
	name { BusFork }
	domain { CGC }
	desc { Copy particles from an input bus to each output bus. }
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	explanation {
To keep the implementation simple, this star does not attempt the
fancy tricks performed by the regular fork star to avoid runtime overhead.
The data values are simply copied at runtime.
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
	state {
	  name {ix}
	  type { int }
	  default { 1 }
	  desc { index for multiple input trace }
	  attributes { A_NONSETTABLE|A_NONCONSTANT }
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
	constructor {
	  noInternalState();
	}
	exectime {
	  // Alert: rough estimate
	  return input.numberPorts();
	}
	go {
	  MPHIter nextin(input), nextoutA(outputA), nextoutB(outputB);
	  PortHole *in, *outA, *outB;
	  ix = 0;
	  while (((in = nextin++) != 0) &&
		 ((outA = nextoutA++) != 0) &&
		 ((outB = nextoutB++) != 0)) {
	    ix = ix+1;
	    addCode("\t$ref(outputA#ix) = $ref(outputB#ix) = $ref(input#ix);");
	  }
	}
}
