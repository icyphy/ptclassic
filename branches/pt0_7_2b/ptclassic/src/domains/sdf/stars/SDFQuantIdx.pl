defstar {
	name { QuantIdx }
	domain { SDF }
	derived { Quant }
	desc {
Quantize the input value to one of N+1 possible output levels using
N thresholds, and output both the quantized result and the
quantization level.  See the Quant star for more information.
	}
	version {@(#)SDFQuantIdx.pl	1.2 2/12/96}
	author { Edward A. Lee, Joseph Buck, and Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	output {
		name {stepNumber}
		type {int}
		desc {Level of the quantization from 0 to N-1}
	}
	// Inherit the setup method
	go {
		SDFQuant :: go();
		stepNumber%0 << int(quantizationLevel);
	}
}
