defstar {
	name {Reciprocal}
	domain {SDF}
	desc { 1/x, with an optional magnitude limit. }
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
This star computes $1/x$, where $x$ is the input.
If the \fImagLimit\fR parameter is not 0.0, then the output is
$+- ~ max ( magLimit, ~ 1/x )$.  In this case, $x$ can be zero
without causing an floating exception.
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
