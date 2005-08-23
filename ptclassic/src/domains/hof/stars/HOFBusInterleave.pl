defstar {
	name {BusInterleave}
	domain {HOF}
	derivedfrom {Base}
	version { @(#)HOFBusInterleave.pl	1.7 11/13/97 }
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
This star interleaves two input busses onto a single bus.
The two input busses must have the same width, which must
be half the width of the output bus.
The input signals are connected to the output in an alternating fashion.
	}

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

	method {
	  name { doExpansion }
	  type { int }
	  code {
	    // Make sure we know the number of connections on the
	    // input and output multiPortHoles.
	    if (! initConnections(output)) return 0;
	    if (! initConnections(top)) return 0;
	    if (! initConnections(bottom)) return 0;

	    MPHIter nexttop(top);
	    MPHIter nextbottom(bottom);
	    // start with the top inputs
	    MPHIter *nexti = &nexttop;
	    MPHIter *previ = &nextbottom;
	    MPHIter nexto(output);

	    PortHole *pi, *po;
	    while ((po = nexto++) != 0) {
	      if ((pi = (*nexti)++) == 0) {
		// Out of inputs, flag error
		Error::abortRun(*this,
				"Not enough inputs for the given number of outputs");
		return 0;
	      }
	      if (!crossConnect(pi, po))
		return 0;
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
	      return 0;
	    }

	    return 1;
	  }
	}
}
