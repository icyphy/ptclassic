defstar {
	name { Average }
	domain { SDF }
	desc {
Average some number of input samples or blocks of input samples.
Blocks of successive input samples are treated as vectors.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
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
		double avg = 0.0;
		for(int j=int(numInputsToAverage)-1; j >= 0; j-- ) {
		    avg += double(input%(j*int(blockSize)+i));
		}
		output%i << avg/int(numInputsToAverage);
	    }
	}
}
