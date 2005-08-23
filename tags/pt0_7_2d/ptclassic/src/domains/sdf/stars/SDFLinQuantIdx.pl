defstar {
	name { LinQuantIdx }
	domain { SDF }
	desc {
The input is quantized to the number of levels given by the "levels"
parameter plus 1.  The quantization levels are uniformly spaced between "low"
and "high" inclusive.  Rounding down is performed, so that output level
will equal "high" only if the input level equals or exceeds "high".
If the input is below "low", then the quantized output will equal "low".
The quantized value is output to the "amplitude" port, while the index
of the quantization level is output to the "stepNumber" port.  This
integer output is useful for stars that need an integer input, such as
Thor stars.
	}
	author { Asawaree Kalavade }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { @(#)SDFLinQuantIdx.pl	1.18 2/25/96 }
	input {
		name {input}
		type {float}
	}
	output {
		name {amplitude}
		type {float}
	}
	output {
		name {stepNumber}
		type {int}
	}
	defstate {
		name {levels}
		type {int}
		default {"128"}
		desc {number of levels to quantize to }
	}
	defstate {
		name {low}
		type {float}
		default {"-3.0"}
		desc {lower limit of signal excursion }
	}
	defstate {
		name {high}
		type {float}
		default {"3.0"}
		desc {upper limit of signal excursion }
	}
	defstate {
		name {height}
		type {float}
		default {"1.0"}
		desc {
height of each quantization step, which is determined by the states
high, low, and levels.
		}
		attributes {A_NONSETTABLE|A_CONSTANT}
	}

	setup {
		if (int(levels) <= 0 ) {
		    Error::abortRun(*this, "levels must be positive");
		}
		else if (double(high) <= double(low)) {
		    Error::abortRun(*this,
				    "quantization range incorrectly ",
				    "specified: high <= low");
		}
		else {
		    height = (double(high) - double(low))/(int(levels) - 0);
		}
	}
	go {
	    	double in = double(input%0);
		double highvalue = double(high);
		double lowvalue = double(low);

	    	if ( in >= highvalue ) {
		    amplitude%0 << highvalue;
                    stepNumber%0 << int(levels) - 1;
		}
		else if ( in <= lowvalue ) {
		    amplitude%0 << lowvalue;
                    stepNumber%0 << 0;
		}
		else {
		    int step = int((in - lowvalue)/double(height));
		    stepNumber%0 << step;
        	    amplitude%0 << double(lowvalue + step * double(height));
		}
	}
}
