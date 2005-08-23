defstar {
	name { AverageCx }
	domain { SDF }
	desc {
Average some number of complex input samples or blocks of complex
input samples. Blocks of successive input samples are treated as vectors.
	}
	version {@(#)SDFAverageCx.pl	1.10	09/01/97}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name { input }
		type { complex }
	}
	output {
		name { output }
		type { complex }
	}
	defstate {
		name { numInputsToAverage }
		type { int }
		default { 8 }
		desc { The number of input samples or blocks to average. }
	}
	defstate {
		name { blockSize }
		type { int }
		default { 1 }
		desc {
Input blocks of this size will be averaged to produce an output block.
		}
	}
	setup {
		input.setSDFParams(int(blockSize)*int(numInputsToAverage),
			int(blockSize)*int(numInputsToAverage)-1);
		output.setSDFParams(int(blockSize), int(blockSize)-1);
	}
	go {
	    for(int i=int(blockSize)-1; i >= 0; i-- ) {
		// We use a temporary variable to avoid gcc2.7.2/2.8 problems
		Complex avg = Complex(0.0);
		for(int j=int(numInputsToAverage)-1; j >= 0; j-- ) {
		    Complex tmpavg = input%(j*int(blockSize)+i);
		    avg += tmpavg;
		}
		output%i << Complex(real(avg)/int(numInputsToAverage),
				    imag(avg)/int(numInputsToAverage));
	    }
	}
}
