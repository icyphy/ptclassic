ident {
#define MAXORDER 1024
}

defstar {
	name { BlockAllPole }
	domain { SDF }
	version {$Id$}
	desc { An all pole filter with coefficients externally supplied. }
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF dsp library }
	explanation {
.pp
This star implements an all pole filter with coefficients that
are periodically updated
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
The maximum filter order (currently 1024) is defined in the source code,
and can be changed, at the expense of recompiling.
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
		double taps[MAXORDER];
		double fdbkDelayLine[MAXORDER];
		int writeIndex;
	}
	start {
	    // set the SDF Params to account for the block processing
	    signalIn.setSDFParams(int(blockSize), int(blockSize)-1);
	    coefs.setSDFParams(int(order), int(order) - 1);
	    signalOut.setSDFParams(int(blockSize), int(blockSize) - 1);
	    writeIndex = 0;
	    for (int i = 0; i < int(order); i++) {
		fdbkDelayLine[i]=0.0;
	    }
	}
	go {
	    int tapsIndex;
	    double out, tap;

	    // first read in new tap values
	    int index = 0;
	    for (int cCount = int(order)-1; cCount >=0; cCount--)
		taps[index++] = float(coefs%cCount);
	
	    // Iterate for each block
	    for (int j = int(blockSize)-1; j >= 0; j--) {

		   out = 0.0;
		   // Compute the inner product.
		   for (int i = 0; i < int(order); i++) {
			out += taps[i]
			    * fdbkDelayLine[(writeIndex + i) % int(order)];
		   }
		   // Get the input and add to the inner product output
		   out += float(signalIn%j);

		   // Feed output back to the delay line:
		   writeIndex--;
		   if(writeIndex < 0) writeIndex = int(order) - 1;
		   fdbkDelayLine[writeIndex] = out;

		   // note: output%0 is the last output chronologically
		   signalOut%(j) << out;
	    }
	}
}
