defstar {
	name {ConvolCx}
	domain {SDF}
	desc {
Convolve two causal finite sequences.
Set truncationDepth larger than the number of output samples of interest.
	}
	version { $Id$ }
	author { Karim-Patrick Khiar }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
This star convolves two causal finite input sequences.
In the current implementation, you should set the truncation
depth larger than the number of output samples of interest.
If it is smaller, you will get unexpected results after truncationDepth
samples.
	}
	seealso { FIR, FIRCx, blockFIR, firDemo }
	input {
		name {inA}
		type {complex}
	}
	input {
		name {inB}
		type {complex}
	}
	output {
		name {out}
		type {complex}
	}
	defstate {
		name {truncationDepth}
		type {int}
		default {256}
		desc { Maximum number of terms in convolution sum. }
	}
	defstate {
		name {iterationCount}
		type {int}
		default {0}
		desc {Count current iteration. }
		attributes { A_NONCONSTANT | A_NONSETTABLE }
	}
	setup {
		inA.setSDFParams(1, int(truncationDepth));
		inB.setSDFParams(1, int(truncationDepth));
	}
	go {
	    Complex &sum =  Complex(0.0);
	    int c = int(iterationCount);
	    for (int k = 0; k < int(truncationDepth); k++) {
		int index = c-k;
		if (index < 0) index += int(truncationDepth);
		sum += Complex(inA%(index)) * Complex(conj(inB%(k)));
	    }
	    out%(0) << sum;
	    if (c >= int(truncationDepth) - 1) iterationCount = 0;
	    else iterationCount = c+1;
	}
}
