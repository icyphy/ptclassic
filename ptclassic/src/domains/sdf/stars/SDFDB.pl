defstar {
	name {DB}
	domain {SDF}
	desc {
Converts input to dB.  Zero and negative values are
converted to "min" (default -100).
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
For inputs that are greater than zero, the output either
$N log sub 10 (input)$ or \fImin\fR, whichever is larger, where
$N ~=~ 10$ if \fIinputIsPower\fR is TRUE, and $N ~=~ 20$ otherwise.
.Id "decibel"
The default is $N ~=~ 20$.
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
	defstate {
		name{inputIsPower}
		type{int}
		default{"FALSE"}
		desc{
TRUE if input is a power measurement, FALSE if it's an amplitude measurement.
		}
	}
	private {
		double gain;
	}
	setup {
		if (int(inputIsPower)) gain=10.0;
		else gain = 20.0;
	}
	go {
		double f = input%0;
		if (f <= 0.0)
			output%0 << double(min);
		else {
			f = gain * log10 (f);
			if (f < double(min)) f = double(min);
			output%0 << f;
		}
	}
}
