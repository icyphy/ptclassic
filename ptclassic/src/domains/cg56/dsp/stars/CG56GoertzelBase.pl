defstar {
	name { GoertzelBase }
	domain { CG56 }
	version { $Id$ }
	desc {
Base class for Goertzel algorithm stars.
	}
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 dsp library }
	input {
		name{input}
		type{fix}
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
		type { fix }
		default { "0.0" }
		desc {
first-order feedback coefficient which is a function of k and N }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name { state1 }
		type { fix }
		default { "0.0" }
		desc { internal state. does not persist across invocations. }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name { state2 }
		type { fix }
		default { "0.0" }
		desc { internal state. does not persist across invocations. }
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name { halfd1 }
		type { fix }
		default { "0.0" }
		desc { internal state. does not persist across invocations. }
		attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
	}
	protected {
		double theta;
	}
	constructor {
		noInternalState();
		theta = 0.0;
	}
	ccinclude { <math.h> }

	method {
	    name {CheckParameterValues}
	    arglist { "()" }
	    type { void }
	    code {
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
	    }
	}

	setup {
		// FIXME: Parameters are not always resolved properly
		// before setup but should be.  For now, check parameters
		// in go method and guard against division by N = 0
		// CheckParameterValues();
		// double Nd = double(int(N));
		double Nd = double(int(N) ? int(N) : 1);
		double kd = double(int(k));
		theta = -2.0 * M_PI * kd / Nd;
		double cosTheta = cos(theta);
		halfd1 = cosTheta;
		d1 = 2.0 * cosTheta;		// can't reuse halfd1, a fix
		input.setSDFParams(int(size), int(size)-1);
	}

	codeblock(init) {
; Goertzel algorithm relies on one feedback coefficient d1 = 2 cos(theta)
; such that theta = 2 Pi k / N for the kth coefficient of an N-point DFT.
; Register usage:
; r0: base address for the block of input samples
; x0: value of one-sample delay (state1)
; x1: value of two-sample delay (state2)
; y0: feedback coefficient d1 divided by 2
; y1: ith input value
	clr	a	#<$addr(input),r0		; a = 0
	clr	b	a,x1	$ref(halfd1),y0		; x1 = 0 and y0 = d1/2
	}

	codeblock(loop,"int len") {
; Begin loop for Goertzel algorithm: run all-pole section for @len iterations
	do	#@len,$label(_GoertzelBase)
	addl	a,b		x:(r0)+,a	; b = a + 2*b and a = x[n]
	sub	x1,a		b,x0		; a = x[n] - state2
$label(_GoertzelBase)
	macr	x0,y0,b		x0,x1		; b = (d1/2)*state1 and x1 = x0
; End loop to compute Goertzel algorithm
	asl	b		a,x0		; b = d1*state2
	}

	go {
		CheckParameterValues();

		int dftLength = int(N);

		// Run all-pole section of Goertzel's algorithm N iterations.
		// Only one multiplier (d1) in iteration.
		// Zero the IIR state for each DFT calculation; otherwise,
		// the filter output could grow without bound.
		// state1 and state2 are states and not local variables
		// ONLY to pass their values to derived stars

		addCode(init);
		addCode(loop(dftLength));
	}

	exectime {
                // FIXME. Estimates of execution time are given in pairs of
		// oscillator cycles because that's the way it was done in
		// Gabriel: they simply counted the number of instructions.

		// However, the do command takes 3 pairs of oscillator cycles
		return (3 + 3 + 3*int(N));
	}
}
