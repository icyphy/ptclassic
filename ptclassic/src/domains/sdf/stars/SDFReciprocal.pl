defstar {
	name {Reciprocal}
	domain {SDF}
	desc {
Output the reciprocal of the input, with an optional magnitude limit.
If the magnitude limit is greater than zero, and the input value is zero,
then the output will equal the magnitude limit.
	}
	version {@(#)SDFReciprocal.pl	1.12 10/07/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
This star computes 1<i>/x</i>, where <i>x</i> is the input.
If the <i>magLimit</i> parameter is not 0.0, then the output is
&#177;max(<i>magLimit</i>,1/<i>x</i>).  In this case, 
<i>x</i> can be zero without causing a floating exception.
The sign of the output is determined by the sign of the input.
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	defstate {
		name{magLimit}
		type{float}
		default{"0.0"}
		desc {If non-zero, limits the magnitude of the output.}
	}
	go {
	   if(double(magLimit) == 0.0)
		output%0 << 1/double(input%0);
	   else
		if(double(input%0) == 0.0)
		    output%0 << double(magLimit);
		else {
		    double t = 1/double(input%0);
		    if (t>double(magLimit))
			output%0 << double(magLimit);
		    else if (t < - double(magLimit))
			output%0 << - double(magLimit);
		    else
			output%0 << t;
		}
	}
}
