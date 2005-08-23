defstar {
	name {DB}
	domain {SDF}
	desc {
Convert input to a decibels (dB) scale.
Zero and negative values are assigned the value "min" (default -100).
The "inputIsPower" parameter should be set to YES if the input signal
is a power measurement (vs. an amplitude measurement).
	}
	version {@(#)SDFDB.pl	1.19 10/07/96}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
For inputs that are greater than zero, the output is either
<i>N</i> log<sub>10</sub>(<i>input</i>) or <i>min</i>, whichever is larger, 
where <i>N</i> = 10 if <i>inputIsPower</i> is TRUE, and <i>N</i> = 20 
otherwise. <a name="decibel"></a>
The default is <i>N</i> = 20.
For inputs that are zero or negative, the output is <i>min</i>.
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
