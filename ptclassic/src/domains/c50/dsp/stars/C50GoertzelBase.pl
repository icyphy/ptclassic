defstar {
	name { GoertzelBase }
	domain { C50 }
	version { $Id$}
	desc {
Base class for Goertzel algorithm stars
	}
	author { Luis Gutierrez, based on CG56 Implementation}
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
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
		name { delays }
		type { fixarray }
		default { 0.0 }
		attributes { A_UMEM|A_NONCONSTANT|A_NONSETTABLE|A_CONSEC }
	}
	defstate {
		name { WnReal }
		type { fix }
		default { "0.0" }
		desc { 
internal state for the storage of the real part of the twiddle factor, 
which is a function of k and N
		}
		attributes { A_UMEM|A_NONCONSTANT|A_NONSETTABLE|A_CONSEC }
	}
	defstate {
		name { WnImag }
		type { fix }
		default { "0.0" }
		desc {
internal state for the storage of the imaginary part of the twiddle factor,
which is a function of k and N
		}
		attributes { A_UMEM|A_NONCONSTANT|A_NONSETTABLE|A_CONSEC }
	}
	protected {
		double theta;
		double temp;
		StringList coeffs;
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
		theta = 2.0 * M_PI * kd / Nd;
		temp = double(cos(theta)/2);
		WnReal = temp;
		WnImag  = double(-sin(theta));
		delays.resize(2);
		input.setSDFParams(int(size), int(size)-1);
	}

	initCode{
		coeffs<<"$starSymbol(cf):\n\t.q15\t";
		coeffs<<temp;
		coeffs<<"\n\t.q15\t-0.5\n$starSymbol(cfe)\n";
	}
		
	codeblock(initialize,"int iter") {
	lacc	#@iter,0
	samm	brcr			; load block repeat counter with iter
	lar	ar0,#$addr(input) 	; ar0 -> input
	lar	ar1,#$addr(delays)	; ar1 -> old samples	
	lar	ar2,#$addr(WnReal)	; ar2 -> twiddle factors(real,imag)
	mar	*,ar1			; make arp -> ar1
	mar	*+			; ar1-> last of old samples
	rptb	$starSymbol(gl)		; block repeat inst.
	zap				; zero prod reg & acc
	rpt	#1			; repeat twice
	macd	cf,*-			; implements part of filter
	lta	*+,ar0			; add previous prod. and set ar0 active
	add	*+,14,ar1		; add input to acc and inc input pointer
	sach	*+,2			; store acc. and make ar1-> last of old samples
$starSymbol(gl):			; end of repeat block; time = 9 cycles
	lt	*,ar2			; treg0 is loaded with w[1]; ar2->twiddleReal/2
	mpy	*+			; P = w[1]*twiddleReal/4; ar2->twiddleImag
	lts	*,ar1			; acc = w[0]/4 + w[1]*twiddleReal/4; treg0=twdlIm
	mpy	*,ar3			; P = w[1]*twiddleIm/2
	}

	go {
		int dftLength = int(N);

		// See the FIXME statement above
		CheckParameterValues();

  		// Initialize registers
		addCode(initialize(dftLength));
	
	}

 	exectime {
		return (4 + 3 + 4*(int(N)));
	}
}
