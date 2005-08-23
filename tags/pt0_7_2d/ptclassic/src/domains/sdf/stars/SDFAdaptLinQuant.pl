defstar {
	name { AdaptLinQuant }
	domain { SDF }
	desc {
Uniform linear quantizer symmetric about 0 with variable step size.
}
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	htmldoc {
The input is quantized to the number of levels given by 2^<i>bits</i>.
The quantization levels are uniformly spaced at the step size given by
the <i>inStep</i> input value and are odd symmetric about zero.
Therefore, the "high" threshold is (2^<i>bits</i> - 1)*<i>inStep</i>/2, and
the "low" threshold is the negative of the "high" threshold.
Rounding to nearest level is performed. Output level will equal "high" only
if the input level equals or exceeds "high".
If the input is below "low", then the quantized output will equal "low".
The quantized value is output on the <i>amplitude</i> port as a floating-point
value, the step size is output on the <i>outStep</i> port as a floating-point
value, and the index of the quantization level on the <i>stepLevel</i> port
as a non-negative integer between 0 and 2^<i>bits</i> - 1, inclusive.
	}
	version { @(#)SDFAdaptLinQuant.pl	1.4	06 Oct 1996 }
	input {
		name {input}
		type {float}
	}
	input {
		name {inStep}
		type {float}
      	}
	output {
		name {amplitude}
		type {float}
	}
	output {
		name {outStep}
		type {float}
	}
	output {
		name {stepLevel}
		type {int}
	}
	defstate {
		name {bits}
		type {int}
	}
	go {
		// check the current value of the stepsize
		double stepsize = double(inStep%0);
		if ( stepsize <= 0.0 ) {
			Error::abortRun(*this, "Non-positive step size");
		}

		// high threshold is ((2^B - 1)/2) * stepsize; low = -high
		// where B is the number of bits
		int numbits = int(bits);
		int twoPowerB = (1 << numbits);
		double high = double(twoPowerB - 1) * stepsize / 2.0;
		double low = -high;

		// compute the quantized output
	    	double in = input%0;
		int quantlevel = 0;
	    	if ( in >= high ) {
			quantlevel = twoPowerB - 1;
			amplitude%0 << high;
		}
		else if (in <= low) {
			quantlevel = 0;
			amplitude%0 << low;
		}
		else {
			quantlevel = int((in-low)/stepsize + 0.5);
        		amplitude%0 << double(low + quantlevel*stepsize);
		}

		outStep%0 << stepsize;
		stepLevel%0 << quantlevel;
	     }
}
