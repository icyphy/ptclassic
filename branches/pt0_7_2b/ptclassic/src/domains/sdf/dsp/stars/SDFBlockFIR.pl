ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 10/31/90

 Implements an FIR filter whose coefficients are periodically updated
 from the outside.  The \fIblockSize\fR parameter tells how often
 the updates occur.  It is an integer specifying how may input samples
 should be processed using each set of coefficients.  The \fIorder\fR
 parameter tells how many coefficients there are.
 The same interpolation and decimation of the FIR star is supported.

 Unfortunately, it proves not too convenient to derive this star from FIR
 because of the changes in the way the inputs and outputs are handled.
 Hence, much of the code here is quite similar to that in the FIR star.
 The maximum filter order (currently 1024) is defined in the source code,
 and can be changed, at the expense of recompiling.

SEE ALSO:
FIR
**************************************************************************/

#define MAXORDER 1024

}

defstar {
	name { BlockFIR }
	domain { SDF }
	desc { "an FIR filter whose coefficients are externally updated" }
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
	}
	defstate {
		name { blockSize }
		type { int }
		default { "128" }
		desc { "Number of inputs that use each each coefficient set" }
	}
	defstate {
		name { order }
		type { int }
		default { "16" }
		desc { "Number of new coefficients to read each time" }
	}
	defstate {
		name {decimation}
		type {int}
		default {1}
		desc {"decimation ratio"}
	}
	defstate {
		name {decimationPhase}
		type {int}
		default {0}
		desc {"downsampler phase"}
	}
	defstate {
		name {interpolation}
		type {int}
		default {1}
		desc {"interpolation ratio"}
	}
	protected {
		int phaseLength;
		double taps[MAXORDER];
	}
	start {
	    int d = decimation;
	    int i = interpolation;
	    int dP = decimationPhase;

	    // Then set the SDF Params to account for the block processing
	    signalIn.setSDFParams(d*int(blockSize),
		d*int(blockSize)+1+int(order)/i);
	    coefs.setSDFParams(int(order), int(order) - 1);
	    signalOut.setSDFParams(i*int(blockSize),
		i*int(blockSize) - 1);
	    if (dP >= d) {
		Error::abortRun (*this, ": decimationPhase too large");
		return;
	    }
	    // The phaseLength is ceiling(order/interpolation)
	    // It is a protected instance variable.
	    phaseLength = int(order) / i;
	    if ((int(order) % i) != 0) phaseLength++;
	}
	go {
	    int phase, tapsIndex;
	    double out, tap;

	    // first read in new tap values
	    int index = 0;
	    for (int cCount = int(order)-1; cCount >=0; cCount--)
		taps[index++] = float(coefs%cCount);
	
	    // Iterate for each block
	    for (int j = int(blockSize)-1; j >= 0; j--) {
	      // phase keeps track of which phase of the
	      //filter coefficients are used.
	      // Starting phase depends on the decimationPhase state.
	      phase = int(decimation) - int(decimationPhase) - 1;   

	      int outCount = int(interpolation)-1;
	
	      // Iterate once for each input consumed
	      for (int inC = 1; inC <= int(decimation); inC++) {
		// Produce however many outputs are required
		// for each input consumed
		while (phase < int(interpolation)) {
		   out = 0.0;
		   // Compute the inner product.
		   for (int i = 0; i < phaseLength; i++) {
			tapsIndex = i * int(interpolation) + phase;
			if (tapsIndex >= int(order))
			    tap = 0.0;
			else
			    tap = taps[tapsIndex];
			out += tap
			    * float(signalIn%(int(decimation) - inC + i
				+ int(decimation)*j));
		   }
		   // note: output%0 is the last output chronologically
		   signalOut%(int(interpolation)*j + outCount--) << out;
		   phase += int(decimation);
		}
		phase -= int(interpolation);
	      }
	    }
	}
}