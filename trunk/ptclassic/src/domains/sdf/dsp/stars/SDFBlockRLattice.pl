defstar {
	name {BlockRLattice}
	domain {SDF}
	desc {
A block Recursive (Backward) (IIR) Lattice filter.
It is identical to the RLattice star, except that
the reflection coefficients are updated each time the star fires
by reading the "coefs" input.
The "order" parameter indicates how many coefficient
should be read.  The "blockSize" parameter specifies how many
signalIn samples should be processed for each set of coefficients.
	}
	version {$Id$}
	author { Alan Kamas and Edward Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	seealso { IIR, Lattice, RLattice, BlockLattice }
	input {
		name {signalIn}
		type {float}
	}
	input {
		name { coefs }
		type { float }
	}
	output {
		name {signalOut}
		type {float}
	}
	defstate {
		name { blockSize }
		type { int }
		default { "128" }
		desc { Number of inputs that use each each coefficient set. }
	}
	defstate {
		name {order}
		type {int}
		default { "16" }
		desc { Number of new coefficients to read each time. }
	}
	protected {
		double *w;
		double *y;
		double *reflectionCoefs;
		int M;	
	}
	constructor {
		w = 0; y = 0; M = -1;
		reflectionCoefs = 0;
	}
	destructor {
		LOG_DEL; delete w;
		LOG_DEL; delete y;
		LOG_DEL; delete reflectionCoefs;
	}
	start {
		// reallocate arrays only if size has changed,
		// or this is the first run.
		if (M != int(order)) {
			M = int(order);
			LOG_DEL; delete w;
			LOG_NEW; w = new double[M+1];
			LOG_DEL; delete y;
			LOG_NEW; y = new double[M+1];
			LOG_DEL; delete reflectionCoefs;
			LOG_NEW; reflectionCoefs = new double[M];
		}
		for (int i=0; i <= M; i++)  w[i]=0.0 ;
		signalIn.setSDFParams(int(blockSize), int(blockSize)-1);
		coefs.setSDFParams(int(order), int(order)-1);
		signalOut.setSDFParams(int(blockSize), int(blockSize)-1);
	}
	go {
	    // first read in new tap values
            int index = 0;
	    for (int cCount = int(order)-1; cCount >=0; cCount--)
		reflectionCoefs[index++] = coefs%cCount;

            // Iterate for each block
            for (int j = int(blockSize)-1; j >= 0; j--) {
		double k;
		
		// Forward:  Compute each of the Y values
		y[0] = double (signalIn%j);   // y(0)=x(n)
		for (int i=1; i <= M; i++) {
			k = reflectionCoefs[M-i];
			y[i] = k * w[i] + y[i-1];
		}
		signalOut%j << y[M];

		// Backward:  Compute the w's for the next round
		for (i = 1; i < M ; i++) {
			k = - reflectionCoefs[M-1-i];
			w[i] = w[i+1] + k*y[i+1];	
		}
		w[M] = y[M];
	    }
	}	
}
