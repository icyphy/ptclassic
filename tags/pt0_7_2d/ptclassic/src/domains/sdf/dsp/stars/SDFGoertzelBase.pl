defstar {
	name { GoertzelBase }
	domain { SDF }
	version { @(#)SDFGoertzelBase.pl	1.6	07/18/96 }
	desc {
Base class for Goertzel algorithm stars.
	}
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	input {
		name{input}
		type{float}
	}
	defstate {
		name { k }
		type { int }
		default { 0 }
		desc { the DFT coefficient to compute (k < N) }
	}
	defstate {
		name { N }
		type { int }
		default { 32 }
		desc { length of the DFT }
	}
	defstate {
		name { size }
		type { int }
		default { 32 }
		desc { amount of data to read (N <= size) }
	}
	defstate {
		name { d1 }
		type { float }
		default { "0.0" }
		desc {
first-order feedback coefficient which is a function of k and N }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name { state1 }
		type { float }
		default { "0.0" }
		desc { internal state. }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name { state2 }
		type { float }
		default { "0.0" }
		desc { internal state. }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	protected {
		double theta;
	}
	constructor {
		theta = 0.0;
	}
	ccinclude { <math.h> }
	setup {
		if ( int(k) < 0 ) {
		  Error::abortRun(*this,
			"The value for state k must be nonnegative.");
		  return;
		}
		if ( int(N) <= 0 ) {
		   Error::abortRun(*this,
			"The value for state N must be positive.");
		   return;
		}
		if ( int(size) <= 0 ) {
		   Error::abortRun(*this,
			"The value for state size must be positive.");
		   return;
		}
		if ( int(k) >= int(N) ) {
		   Error::abortRun(*this,
			"The DFT coefficient k must be less than the ",
			"DFT length N.");
		   return;
		}
		if ( int(N) > int(size) ) {
		   Error::abortRun(*this,
			"The DFT length N must be less than or equal to the ",
			"number of data samples read, given by state size.");
		   return;
		}
		double kd = int(k);
		double Nd = int(N);
		theta = 2.0 * M_PI * kd / Nd;
		d1 = 2.0 * cos(theta);
		input.setSDFParams(int(size), int(size)-1);
	}
	go {
		// Run all-pole section of Goertzel's algorithm N iterations.
		// Only one multiplier (d1) in iteration.
		// Zero the IIR state for each DFT calculation; otherwise,
		// the filter output could grow without bound.
		// state1 and state2 are states and not local variables
		// ONLY to pass their values to derived stars
		state1 = 0.0;
		state2 = 0.0;
		double acc = 0.0;
		double d1val = d1;
		for (int i = int(N)-1; i >= 0; i--) {
		  acc = double(input%i);
		  acc += d1val * double(state1);
		  acc -= double(state2);
		  state2 = double(state1);
		  state1 = acc;
		}
	}
}
