defstar {
	name {BusSplit}
	domain {HOF}
	derivedfrom {Base}
	version { @(#)HOFBusSplit.pl	1.10 11/13/97 }
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
This star splits an input bus into two.
If the input bus width is N, and the output bus widths
are M1 and M2, then we require that N = M1 + M2.
The first M1 inputs go the first output bus, while
the next M2 inputs go to the second output bus.
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
	    MPHIter nexti(input);

	    PortHole *pi, *po;
	    while ((pi = nexti++) != 0) {
	      if ((po = (*nexto)++) == 0) {
		// Out of outputs, switch to the bottom outputs.
		nexto = &nextbottom;
		if ((po = (*nexto)++) == 0) {
		  // Still out of outputs
		  Error::abortRun(*this,
				  "Not enough outputs for the given number of inputs");
		  return 0;
		}
	      }
	      if (!crossConnect(pi, po))
		return 0;
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
