defstar {
	name { AdaptLinQuant }
	domain { CGC }
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
	version { $Id$ }
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
		default { "8" }
	}
	initCode {
		addInclude("<stdio.h>");
	}
	go {
		addCode(adaptlinquant);
	}
	codeblock(adaptlinquant) {
		double stepsize = (double) $ref(inStep);
		int numbits, twoPowerB, quantLevel;
		double high, low, in;

	        /* check the current value of the stepsize */
	        if ( stepsize <= 0.0 ) {
			fprintf(stderr, "Error: Non-positive step size for AdaptLinQuant star \n");
			exit(1);
		}

	        /* high threshold is ((2^B - 1)/2) * stepsize; low = -high
		   where B is the number of bits */
		numbits = (int) $val(bits);
		twoPowerB = (1 << numbits);
		high = ((double)(twoPowerB - 1)) * stepsize/ 2.0;
		low = -high;

		/* compute the quantized output */
	    	in = $ref(input);
		quantLevel = 0;

	    	if ( in >= high ) {
			quantLevel = twoPowerB - 1;
			$ref(amplitude) = high;
		}
		else if (in <= low) {
			quantLevel = 0;
			$ref(amplitude) = low;
		}
		else {
			quantLevel = (int)((in-low)/stepsize + 0.5);
        		$ref(amplitude) = (double)(low + quantLevel*stepsize);
		}

		$ref(outStep) = stepsize;
		$ref(stepLevel) = quantLevel;
	     }
}
