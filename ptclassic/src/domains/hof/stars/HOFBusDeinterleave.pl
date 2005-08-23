defstar {
	name {BusDeinterleave}
	domain {HOF}
	derivedfrom {Base}
	version { @(#)HOFBusDeinterleave.pl	1.6 11/13/97 }
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
This star deinterleaves a bus, producing two output busses of equal width.
The input bus must have even width.
The even numbered input signals are connected to the
first output bus, while the odd numbered input signals
are connected to the second output bus.
	}

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

	method {
	  name { doExpansion }
	  type { int }
	  code {
	    // Make sure we know the number of connections on the
	    // input and output multiPortHoles.
	    if (! initConnections(top)) return 0;
	    if (! initConnections(bottom)) return 0;
	    if (! initConnections(input)) return 0;

	    MPHIter nexttop(top);
	    MPHIter nextbottom(bottom);
	    // start with the top outputs
	    MPHIter *nexto = &nexttop;
	    MPHIter *prevo = &nextbottom;
	    MPHIter nexti(input);

	    PortHole *pi, *po;
	    while ((pi = nexti++) != 0) {
	      if ((po = (*nexto)++) == 0) {
		// Out of outputs, flag error
		Error::abortRun(*this,
				"Not enough outputs for the given number of inputs");
		return 0;
	      }
	      if (!crossConnect(pi, po))
		return 0;
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
	      return 0;
	    }

	    return 1;
	  }
	}
}
