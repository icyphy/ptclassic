defstar {
	name { PcwzLinear }
	domain { SDF }
	desc {
This star implements a piecewise linear mapping from the
input to the output.  The mapping is given by a sequence
of (x,y) pairs, which specify the breakpoints in the function.
The sequence of x values must be increasing. The function
implemented by the star can be represented by drawing straight
lines between the (x,y) pairs, in sequence.  The default
mapping is the "tent" map, in which inputs between -1.0
and 0.0 are linearly mapped into the range -1.0 to 1.0.
Inputs between 0.0 and 1.0 are mapped into the same range,
but with the opposite slope, 1.0 to -1.0. If the input is
outside the range specified in the "x" values of the breakpoints,
then the appropriate extreme value will be used for the output.
Thus, for the default map, if the input is -2.0, the output
will be -1.0.  If the input is +2.0, the output will again be -1.0.
	}
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
.Id "table lookup"
.Id "piecewise linear map"
.Id "tent map"
.Id "linear map"
	}
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { breakpoints }
		type { complexarray }
		default { "(-1.0, -1.0) (0.0, 1.0) (1.0, -1.0)" }
		desc { "Endpoints and breakpoints in the mapping" }
	}
	begin {
	  // Check to make sure that x values are increasing
	  double previous = breakpoints[0].real();
	  for(int i = 1; i < breakpoints.size(); i++) {
	    if (breakpoints[i].real() <= previous) {
	      Error::abortRun(*this,"Breakpoints are not increasing in x");
	      return;
	    } else {
	      previous = breakpoints[i].real();
	    }
	  }
	}
	go {
	  // FIXME: Linear search on X values is expensive.
	  double in = double(input%0);
	  if (in < breakpoints[0].real()) {
	    output%0 << breakpoints[0].imag();
	    return;
	  }
	  for (int i = 1; i < breakpoints.size(); i++) {
	    if (in < breakpoints[i].real()) {
	      double x1 = breakpoints[i-1].real();
	      double y1 = breakpoints[i-1].imag();
	      double x2 = breakpoints[i].real();
	      double y2 = breakpoints[i].imag();
	      output%0 << y1 + (y2 - y1)*(in - x1)/(x2 - x1);
	      return;
	    }
	  }
	  output%0 << breakpoints[breakpoints.size() - 1].imag();
	}
}
