defstar {
	name { Average }
	domain { SDF }
	desc {
Averages some number of input samples or blocks of input samples.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
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
	start {
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
