defstar {
	name {DB}
	domain {SDF}
	desc {
Converts input to dB.  Zero and negative values are
converted to 'min' (default -100).
	}
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
For inputs that are greater than zero, the output either
$20 log sub 10 (input)$ or \fImin\fR, whichever is larger.
For inputs that are zero or negative, the output is \fImin\fR.
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
		name{min}
		type{float}
		default{"-100"}
		desc{Minimum output value.}
	}
	go {
		float f = input%0;
		if (f <= 0.0)
			output%0 << double(min);
		else {
			f = 20.0 * log10 (f);
			if (f < double(min)) f = double(min);
			output%0 << f;
		}
	}
}
