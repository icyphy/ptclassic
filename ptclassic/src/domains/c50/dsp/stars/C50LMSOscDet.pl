defstar {
	name { LMSOscDet }
	domain { C50 }
 	derivedFrom { FIR }
	desc {
This filter tries to lock onto the strongest sinusoidal component in
the input signal, and outputs the current estimate of the cosine
of the frequency of the strongest component.  It is a three-tap
Least-Mean Square (LMS) filter whose first and third coefficients are
fixed at one.  The second coefficient is adapted.  It is a normalized
version of the Direct Adaptive Frequency Estimation Technique.
	}
	version { $Id$ }
	author { Luis Gutierrez }
	acknowledge { Brian L. Evans, author of the CG56 version }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	explanation {
The initial taps of this LMS filter are 1, -2, and 1.  The second tap
is adapted while the others are held fixed.  The second tap is equal
to $-2 a sub 1$, and its adaptation has the form
.EQ
y[n] = x[n] - 2 a sub 1[k] x[n-1] + x[n - 2]
a sub 1[k] = a sub 1 [k-1] + 2 mu e[n] x[n-1]
.EN
where $y[n]$ is the output of this filter which can be used as the
error signal.  The step size term $mu$ is fixed by the value of
the \fIstepSize\fR parameter.  You can effectively vary the step size
by attenuating the error term as
.EQ
e[n] = {{y[n]} over {k}}
.EN
assuming that k = 1, 2, 3, and so forth.  When the error becomes relatively
small, this filter gives an estimate of the strongest sinusoidal component:
.EQ
a sub 1 = cos omega
.EN
This filter outputs the current value of $a sub 1$ on the \fIcosOmega\fR
output port.  The initial value is $a sub 1 = 1$, that is, zero frequency,
so the initial value of the second tap is -2.
.PP
For more information on the LMS filter implementation, see the description
of the LMS star upon which this star derived.
.Id "Direct Adaptive Frequency Estimation"
	}
	seealso {FIR, LMS}

	// Since we're not derived from C50LMS, declare an error port
        input {
		name { error }
		type { fix }
		}
	// Unique port for LMSOscDet
	output {
		name { cosOmega }
		type { fix }
		desc {
Current estimated value of the cosine of the frequency of the dominate
sinusoidal component of the input signal.
		}
	}
	// FIXME: Remove these states when C50LMS is derived from C50FIR
	defstate {
		name { stepSize }
		type { fix }
		default { "0.01" }
		desc { Adaptation step size. }
	}
	defstate {
		name { errorDelay }
		type { int }
		default { "1" }
		desc {  Delay in the update loop. }
	}


	// State specific to LMSOscDet

	defstate {
		name { initialOmega }
		type { float }
		default { "PI/4" }
		desc {
The initial guess at the angle being estimated in radians.
		     }
	}

	defstate {
		name { delayBuf }
		type { fixarray }
		default { ""}
		desc { 
Buffer to implement delays > 1 
		}
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}

	defstate {
		name { delayPtr }
		type { int }
		default { 0 }
		desc {
Pointer to next element to be stored in delayBuf. Needed only in delay > 1
		}
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}

	constructor {
		// taps state: not constant, length three, and not settable
		taps.clearAttributes(A_CONSTANT);
		taps.clearAttributes(A_SETTABLE);

		// decimation is not supported
		decimation.clearAttributes(A_SETTABLE);
		decimationPhase.clearAttributes(A_SETTABLE);

	}

	initCode {
		if (int(errorDelay) > 1)   addCode(initDelayPtr);
		C50FIR::initCode(); // call initCode method of FIR filter
	}

        method {
	    name {CheckParameterValues}
	    arglist { "()" }
	    type { void }
	    code {
		if ( (stepSize) <= 0.0 ) {
		    Error::abortRun(*this,
				    "The step size must be positive");
		    return;
		}
		if ( (int(errorDelay) < 1) || (int(errorDelay) > 255) ) {
		    Error::abortRun(*this,
				    "The error delay must be in the range [1,255]");
		    return;
		}
	    }
	}

	protected{
		//intMu is a 16 bit representation of stepSize
		int intMu;
		// delayMask is used to implement modulo addressing
		int delayMask;
	}

	setup {
		// if delay requested is larger than 1 allocate 
		// memory for delayBuf and delayPtr

		if (int(errorDelay) > 1) {
			delayBuf.setAttributes(A_UMEM|A_CIRC);
			delayPtr.setAttributes(A_UMEM);
			int buffSize = 1;
			int delaySize = errorDelay - 1;
		//to support modulo addressing delayBuf must be
		// 2^k words long.
			for ( int i = 0; i < 8; i++){
				if ( buffSize >= delaySize) break;
				buffSize = 2*buffSize;
			}
			delayBuf.resize(buffSize);
			delayMask = buffSize - 1;
		}

                // FIXME: Parameters are not always resolved properly
		// before setup but should be.  For now, check parameters
		// in go method.
		// CheckParameterValues();
	
		// we don't support decimation: make sure that it's 1
		decimation = 1;

		// initialize the taps of the three-tap LMS FIR filter
		taps.resize(3);
		taps[0] =  0.5;
		taps[1] = -cos(double(initialOmega));
		taps[2] =  0.5;
	
		double temp = stepSize.asDouble();
		if (temp >= 0) {
			intMu = int(32768*temp + 0.5);
		} else {
			intMu = int(32768*(1-temp) + 0.5);
		}

		// call the LMS FIR filter setup method
		C50FIR::setup();

	}

	codeblock(initDelayPtr){
	.ds	$addr(delayPtr)
	.word	$addr(delayBuf)
	.text
	}

	codeblock(initialCode) {
	ldp	#00h			; data page pointer = 0
	setc	ovm			; set overflow mode
	lar	ar1,#$addr(oldSamples,1) ; ar1-> x[n-1]
	lar	ar2,#$addr(cosOmega)	; ar2->cosOmega
	lacl	#$starSymbol(cfs)	; acc -> taps[1]
	add	#1			; acc++
	samm	bmar			; BMAR -> taps[1]
* taps for FIR are stored in program memory(at label $starSymbol(cfs))!!
	blpd	bmar,ar3		; ar3 = taps[1]
	mar	*,ar1			; arp = 1
	}

// this star is generally used with the diamond shaped delay symbol
//attached to the feedback path leading to the error input. Only 
//delays of 1 are currently supported with this symbol so a value
//of 1 for the delay means do nothing as far as this star is
//concerned.  For larger values of delays additional code is requird

	codeblock(delayOne){
	lmmr	treg0,#$addr(error)	; treg0 = error
	}

	codeblock(delayStd,""){
	mar	*,ar4
	lacl	#@delayMask
	samm	dbmr
	lmmr	ar4,#$addr(delayPtr)
	bldd	#$addr(error),*+
	apl	ar4			; the apl/opl thing is there because
	opl	#$addr(delayBuf),ar4	; the C50 doesn't support modulo
	smmr	ar4,#$addr(delayPtr)	; addressing directly.
	sbrk	#$val(errorDelay)	; 0< erroDelay < 256 !!
	apl	ar4
	opl	#$addr(delayBuf),ar4
	lt	*,ar1
	}
	
	codeblock(finalCode,""){
	mpy	#@intMu			; p = 0.5*error*Mu
	ltp	*,ar2			; acc = 0.5*error*Mu, treg0 = x[n-1]
	sach	*,3			; cosOmega = 4*error*Mu
	mpy	*			; p = 2*error*Mu*x[n-1]
	lacc	ar3,15			; acc = 0.5*taps[1](old)
	spac				; acc = 0.5(taps[1](old)-4*error*Mu*x[n-1])
	sach	ar3,1			; ar3 = 2*acc
	neg				; acc = -acc
	sach	*,1			; cosOmega = -2(acc)
	bldp	ar3			; taps[1] = newtap
	}


	go {
		CheckParameterValues();

		// Update second tap and compute the output

		addCode(initialCode);
		if (int(errorDelay) > 1) addCode(delayStd());
		else addCode(delayOne);
		addCode(finalCode());


		// Run the FIR filter
		C50FIR :: go();
	}
	// Inherit the wrapup method

	exectime {
		return 20 + C50FIR::myExecTime();
	}

}

