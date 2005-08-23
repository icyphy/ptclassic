defstar {
	name { BusFork }
	domain { CGC }
	desc { Copy particles from an input bus to each output bus. }
	version {@(#)CGCBusFork.pl	1.6   01 Oct 1996}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	htmldoc {
To keep the implementation simple, this star does not attempt the
fancy tricks performed by the regular fork star to avoid runtime overhead.
The data values are simply copied at runtime.
        }
	location { CGC main library }
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
	  int port = 0;
	  while (((in = nextin++) != 0) &&
		 ((outA = nextoutA++) != 0) &&
		 ((outB = nextoutB++) != 0)) {
	    port++;
	    @	$ref(outputA#@port) = $ref(outputB#@port) = $ref(input#@port);
	  }
	}
}
