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
	protected {
		double theta;
	}
	constructor {
		noInternalState();
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
		theta = -2.0 * M_PI * kd / Nd;
		d1 = 2.0 * cos(theta);
		input.setSDFParams(int(size), int(size)-1);
	}

	codeblock(init) {
; move constant d1 into y0 where d1 = 2 cos(theta) such that
; theta = 2 Pi k / N for the kth coefficient of an N-point DFT
; r0: base address for the block of input samples
; x0: value of one-sample delay (state1)
; x1: value of two-sample delay (state2)
		clr	a	#<$addr(input),r0	#$val(d1),y0
		clr	b	a,x1
	}

	codeblock(loop,"int len") {
; Run all-pole section of Goertzel's algorithm N iterations.
		do	#@len,$label(_GoertzelBase)	; begin loop
		move	x:(r0)+,a	a,x0		; save previous a
		mac	x0,y0,a
$label(_GoertzelBase)
		sub	x1,a		x0,x1		; end loop
		mac	y0,x1,b		a,x0		; b = d1*state2
	}

	go {
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
		// The do command takes 6 oscillator cycles
		// However, the exectime is given in half oscillator cycles
		// Why?  Because that's the way it was done in Gabriel
		return (4 + 3*int(N));
	}
}
