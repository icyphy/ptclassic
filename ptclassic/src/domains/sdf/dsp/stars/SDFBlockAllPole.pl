defstar {
	name { BlockAllPole }
	domain { SDF }
	version {$Id$}
	desc { An all pole filter with coefficients externally supplied. }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
.pp
This star implements an all pole filter with coefficients that
are periodically updated
.Id "allpole filter, block"
.Id "filter, allpole, block"
.Id "filter, IIR, block"
from the outside.  The \fIblockSize\fR parameter tells how often
the updates occur.  It is an integer specifying how may input samples
should be processed using each set of coefficients.  The \fIorder\fR
parameter tells how many coefficients there are.
The transfer function of the filter is
.EQ
H(z) ~=~ 1 over { 1 ~-~ z sup -1 D(z) }
.EN
where $D(z)$ is the specified by the externally supplied coefficients.
Let
.EQ
D(z) ~=~ d sub 1 ~+~ d sub 2 z sup -1 ~+~ ... ~+~ d sub M z sup M-1 ~.
.EN
Then $d sub i$ is the $i sup th$ coefficient supplied on the
\fIcoefs\fR input.
.pp
No decimation or interpolation is supported.
	}
	seealso { FIR BlockFIR }
	input {
		name {signalIn}
		type {float}
	}
	output {
		name {signalOut}
		type {float}
	}
	input {
		name { coefs }
		type { float }
		desc { Coefficients of the denominator polynomial }
	}
	defstate {
		name { blockSize }
		type { int }
		default { "128" }
		desc { Number of inputs that use each each coefficient set. }
	}
	defstate {
		name { order }
		type { int }
		default { "16" }
		desc { Number of new coefficients to read each time. }
	}
	protected {
		double *taps;
		double *fdbkDelayLine;
		int M;
		int writeIndex;
	}
	constructor {
	    taps = 0;
	    M = 0;
	    fdbkDelayLine = 0;
	}
	destructor {
	    LOG_DEL; delete taps;
	    LOG_DEL; delete fdbkDelayLine;
	}
	start {
	// delete and remake arrays if a different size is being used.
	    if (int(order) != M) {
		LOG_DEL; delete taps;
		LOG_DEL; delete fdbkDelayLine;
		M = int(order);
		LOG_NEW; taps = new double[M];
		LOG_NEW; fdbkDelayLine = new double[M];
	    }
	    // set the SDF Params to account for the block processing
	    signalIn.setSDFParams(int(blockSize), int(blockSize)-1);
	    coefs.setSDFParams(M, M - 1);
	    signalOut.setSDFParams(int(blockSize), int(blockSize) - 1);
	    writeIndex = 0;
	    for (int i = 0; i < M; i++) {
		fdbkDelayLine[i]=0.0;
	    }
	}
	go {
	    double out;

	    // first read in new tap values
	    int index = 0;
	    for (int cCount = M-1; cCount >=0; cCount--)
		taps[index++] = coefs%cCount;
	
	    // Iterate for each block
	    for (int j = int(blockSize)-1; j >= 0; j--) {

		   out = 0.0;
		   // Compute the inner product.
		   for (int i = 0; i < M; i++) {
			out += taps[i]
			    * fdbkDelayLine[(writeIndex + i) % M];
		   }
		   // Get the input and add to the inner product output
		   out += double(signalIn%j);

		   // Feed output back to the delay line:
		   writeIndex--;
		   if(writeIndex < 0) writeIndex = M - 1;
		   fdbkDelayLine[writeIndex] = out;

		   // note: output%0 is the last output chronologically
		   signalOut%(j) << out;
	    }
	}
}
