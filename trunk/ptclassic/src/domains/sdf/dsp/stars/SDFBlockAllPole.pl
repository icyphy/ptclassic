defstar {
	name { BlockAllPole }
	domain { SDF }
	version {$Id$}
	desc {
This star implements an all pole filter with the denominator
coefficients of the transfer function externally supplied.
For each set of coefficients, a block of input samples is processed,
all in one firing.
	}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	htmldoc {
This star implements an all-pole filter with coefficients that
are periodically updated
<a name="all-pole filter, block"></a>
<a name="filter, all-pole, block"></a>
<a name="filter, IIR, block"></a>
from the outside.  The <i>blockSize</i> parameter tells how often the
updates occur.  This integer parameter specifies how may input samples
should be processed using each set of coefficients.  The <i>order</i>
parameter tells how many coefficients there are.
The transfer function of the filter is
<pre>
H(z) = 1/(1 - z<sup>-1</sup>D(z))
</pre>
where <i>D</i>(<i>z</i>) is specified by the externally supplied coefficients.
Let
<pre>
D(z) = d<sub>1</sub> + d<sub>2</sub>z<sup>-1</sup> + ... + d<sub>M</sub>z<sup>M-1</sup>.
</pre>
Then <i>d <sub>i</sub></i> is the <i>i <sup>th</sup></i> coefficient supplied on the
<i>coefs</i> input.
<p>
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
	    LOG_DEL; delete [] taps;
	    LOG_DEL; delete [] fdbkDelayLine;
	}
	setup {
	// delete and remake arrays if a different size is being used.
	    if (int(order) != M) {
		LOG_DEL; delete [] taps;
		LOG_DEL; delete [] fdbkDelayLine;
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
