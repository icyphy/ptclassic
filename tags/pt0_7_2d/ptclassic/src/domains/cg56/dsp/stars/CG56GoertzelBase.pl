defstar {
	name { GoertzelBase }
	domain { CG56 }
	version { @(#)CG56GoertzelBase.pl	1.15	07/18/96 }
	desc {
Base class for Goertzel algorithm stars
	}
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
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
		name { WnReal }
		type { fix }
		default { "0.0" }
		desc {
internal state for the storage of the real part of the twiddle factor, 
which is a function of k and N
		}
		attributes { A_XMEM|A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name { WnImag }
		type { fix }
		default { "0.0" }
		desc {
internal state for the storage of the imaginary part of the twiddle factor,
which is a function of k and N
		}
		attributes { A_YMEM|A_NONCONSTANT|A_NONSETTABLE }
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
	    name{CheckParameterValues}
	    arglist{  "()" }
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
		  	"The DFT lenght N must be less that or equal to the ",
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
		WnReal= double(cos(theta));
		WnImag  = double(sin(theta));
		input.setSDFParams(int(size), int(size)-1);
	}

	codeblock(initialize) {
; Register usage:
; x0 = real part of previous output 
; x1 = WnReal
; y0 = imaginary part of previous output
; y1 = WnImag
; a = real part of this output
; b = imaginary part of this output
; r0 = address of real part of input
;
; Initialization
; b = x0 = y0 = 0
	clr	a	#$addr(input),r0		; r0 = address of input
	clr	b	$ref(WnReal),x1		a,y0	; x1 = WnReal
	clr	a	b,x0		$ref(WnImag),y1	; y1 = WnImag
	move 	x:(r0)+,a			; a = real part of first input
	}	

	codeblock(loop,"int len"){
; Compute the output of the Goertzel filter
	do	#@len,$label(_GoertzelLoop)
	macr	x1,x0,a		b,y0	
	macr	-y1,y0,a	#0,b		
	macr	x0,y1,b		a,x0
	macr	x1,y0,b		x:(r0)+,a
$label(_GoertzelLoop)
; Register usage:
; x0 = real coefficient
; b = imaginary coefficient
	}

	go {
		int dftLength = int(N);

		// See the FIXME statement above
		CheckParameterValues();

  		// Initialize registers
		addCode(initialize);

		// Compute kth coeff discarding all but last sample
		addCode(loop(dftLength));
	}

	exectime {
		return (4 + 3 + 4*(int(N)));
	}
}
