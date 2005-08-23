defstar {
	name {BusMerge}
	domain {HOF}
	derivedfrom {Base}
	version { @(#)HOFBusMerge.pl	1.10 11/13/97 }
	author { Edward A. Lee, Tom Lane  }
	location { HOF main library }
	copyright {
Copyright (c) 1994-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	desc {
Bridge inputs to outputs and then self-destruct.
This star merges two input busses into a single bus.
If the input bus widths are M1 and M2, and the output
bus width is N, then we require that N = M1 + M2.
The first M1 outputs come from the first input bus,
while the next M2 outputs come from the second input bus.
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
	    MPHIter nexto(output);

	    PortHole *pi, *po;
	    while ((po = nexto++) != 0) {
	      if ((pi = (*nexti)++) == 0) {
		// Out of inputs, switch to the bottom inputs.
		nexti = &nextbottom;
		if ((pi = (*nexti)++) == 0) {
		  // Still out of inputs
		  Error::abortRun(*this,
				  "Not enough inputs for the given number of outputs");
		  return 0;
		}
	      }
	      if (!crossConnect(pi, po))
		return 0;
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
