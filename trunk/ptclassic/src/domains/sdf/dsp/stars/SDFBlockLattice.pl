defstar {
	name {BlockLattice}
	domain {SDF}
	desc {
A Block Forward Lattice filter.
It is identical to the Lattice star except that
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
	seealso { Lattice, RLattice, BlockRLattice, FIR, FIRCx, Biquad}
	input {
		name {signalIn}
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
		desc { Number of inputs that use each each coefficient set. }
	}
	defstate {
		name { order }
		type { int }
		default { "16" }
		desc { Number of new coefficients to read each time. }
	}
	output {
		name {signalOut}
		type {float}
	}
	protected {
		double *b;
		double *f;
		double *reflectionCoefs;
		int lastM;	
	}
	constructor {
		b = 0; f = 0; lastM = -1;
		reflectionCoefs = 0;
	}
	destructor {
		LOG_DEL; delete b;
		LOG_DEL; delete f;
		LOG_DEL; delete reflectionCoefs;
	}
	setup {
		// reallocate arrays only if size has changed,
		// or this is the first run.
		if (lastM != int(order)) {
			lastM = int(order);
			LOG_DEL; delete b;
			LOG_NEW; b = new double[lastM];
			LOG_DEL; delete f;
			LOG_NEW; f = new double[lastM+1];
			LOG_DEL; delete reflectionCoefs;
			LOG_NEW; reflectionCoefs = new double[lastM];
		}
		for (int i=0; i < lastM; i++)  b[i]=0.0 ;
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
		
		// Forward prediction error
		f[0] = double (signalIn%j);   // f(0)=x(n)
		for (int i=1; i <= lastM; i++) {
			k = - reflectionCoefs[i-1];
			f[i] = k * b[i-1] + f[i-1];
		}
		signalOut%j << f[lastM];

		// Backward:  Compute the w's for the next round
		for (i = lastM-1; i >0 ; i--) {
			k = - reflectionCoefs[i-1];
			b[i] = k * f[i-1] + b[i-1];	
		}
		b[0] = double (signalIn%j);   // b[0]=x[n] 
	    }
	}	
}
