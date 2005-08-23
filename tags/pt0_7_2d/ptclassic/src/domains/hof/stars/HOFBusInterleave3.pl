defstar {
	name {BusInterleave3}
	domain {HOF}
	derivedfrom {Base}
	desc {
Bridge inputs to outputs and then self-destruct.
This star interleaves three input busses onto a single bus.
The three input busses must have the same width, which must
be one-third the width of the output bus.
The input signals are connected to the output in an alternating fashion.
	}
	version {@(#)HOFBusInterleave3.pl	1.1 11/10/97}
	author { E. A. Lee, Tom Lane }
	copyright {
Copyright (c) 1994-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { HOF main library }
	outmulti {
		name {output}
		type {anytype}
	}
	inmulti {
	        name {top}
		type {anytype}
	}
	inmulti {
	        name {middle}
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
	    if (! initConnections(middle)) return 0;
	    if (! initConnections(bottom)) return 0;

	    // Build iterators.  This'd be easier if we could use an
	    // array of MPHIters, but there's no default constructor.
	    MPHIter* nexti[3];
	    MPHIter nexttop(top);
	    nexti[0] = &nexttop;
	    MPHIter nextmiddle(middle);
	    nexti[1] = &nextmiddle;
	    MPHIter nextbottom(bottom);
	    nexti[2] = &nextbottom;
	    int inindex = 0;	// start with the top inputs
	    MPHIter nexto(output);

	    PortHole *pi, *po;
	    while ((po = nexto++) != 0) {
	      if ((pi = (*nexti[inindex])++) == 0) {
		// Out of inputs, flag error
		Error::abortRun(*this,
				"Not enough inputs for the given number of outputs");
		return 0;
	      }
	      if (!crossConnect(pi, po))
		return 0;
	      if (++inindex >= 3)
		inindex = 0;
	    }                        // while loop

	    // Out of outputs at this point.
	    // Check to be sure we are also out of inputs.
	    if (nexttop++ != 0 ||
		nextmiddle++ != 0 ||
		nextbottom++ != 0) {
	      Error::abortRun(*this,
			      "Not enough outputs for the given number of inputs");
	      return 0;
	    }

	    return 1;
	  }
	}
}
