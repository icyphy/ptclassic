defstar {
	name { LMSOscDet }
	domain { C50 }

// 	derivedFrom { FIR }
// this star is derived from FIR in the cg56 domain but in the 50
// domain the coefficients for FIR are stored in program memory.
// the dsk assembler does not accept an statement like
// lmmr bmar,#<label pointing to 1st tap> so the only other way 
// to modify the second tap is to do
// rptb <label pointing to 1st tap> which loads the address of
// first tap into paer reg. and then do lmmr bmar,#paer to 
// get bmar to point to second tap.  One could then use bldp/blpd
// instructions to modify the second tap.  Because of all these
// instructions are expensive it's not convenient to derive this
// star from FIR.

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
a sub 1[k] = a sub 1 [k-1] + 2 mu e[n-1] x[n-1]
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

	input {
		name { signalIn }
		type { fix }
	}

	output {
		name { signalOut }
		type { fix }
	}


	// If input is 0 then the error output will be 0 so
	// to prevent false detections this star will set
	// the value of tap to 0.70710 when the power level
	// drops below the "powerThreshold" value.  Doing this
	// will produce a cos value which corresponds to an invalid
	// DTMF tone.
	input {
		name { powerLevel }
		type { fix }
	}


	state {
		name { powerThreshold }
		type { fix }
		default { 0.0 }
	}

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
		name { lmsOldSamples }
		type { fixarray }
		default { "0.0 0.0 0.0 " }
		desc {
Buffer to hold old samples, since the star is not derived
from FIR.
		}
		attributes { A_NONSETTABLE| A_NONCONSTANT | A_UMEM }
	}
	
	defstate {
		name { tap }
		type { fix }
		default { 0.707106781 }
		desc {
Tap that's being adapted.
		}
		attributes { A_NONSETTABLE| A_NONCONSTANT | A_UMEM }
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

	initCode {
		if (int(errorDelay) > 1)   addCode(initDelayPtr);
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
		// parameter to keep track of exec time
		int time;
		// power threshold parameter as integet
		int thresholdAsInt;
	}

	setup {
		// if delay requested is larger than 1 allocate 
		// memory for delayBuf and delayPtr
		
		time = 23;

		if (int(errorDelay) > 1) {
			time += 12;
			delayBuf.setAttributes(A_UMEM|A_CIRC);
			delayPtr.setAttributes(A_UMEM);
			int buffSize = 1;
			int delaySize = errorDelay - 1;
		//to support modulo 2 addressing delayBuf must be
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
	
		tap = -cos(double(initialOmega));
		
		lmsOldSamples.resize(2);
	
		double temp = stepSize.asDouble();
		if (temp >= 0) {
			intMu = int(32768*temp + 0.5);
		} else {
			intMu = int(32768*(2+temp) + 0.5);
		}
		
		temp = powerThreshold.asDouble();
	
		if (temp >= 0) {
			thresholdAsInt = int(32768*temp + 0.5);
		} else {
			thresholdAsInt = int(32768*(2+temp) + 0.5);
		}

	}

	codeblock(initDelayPtr){
	.ds	$addr(delayPtr)
	.word	$addr(delayBuf)
	.text
	}

	codeblock(initialCode,"") {
	lar	ar5,#$addr(powerLevel)
	lar	ar4,#$addr(signalOut)
	lar	ar1,#$addr(lmsOldSamples,1)
	lar	ar2,#$addr(cosOmega)
	lar	ar3,#$addr(tap)
	mar	*,ar5
	lacc	*,0,ar3			; accL = powerLevel
	sub	#@(thresholdAsInt),0	; accL = powerLevel - powerThreshold
	bcndd	$starSymbol(skip),leq
	lacc	#42366,15
	}

// this star is generally used with the diamond shaped delay symbol
//attached to the feedback path leading to the error input. Only 
//delays of 1 are currently supported with this symbol so a value
//of 1 for the delay means do nothing as far as this star is
//concerned.  For larger values of delays additional code is requird.

	codeblock(delayOne){
	mar	*,ar1
	lmmr	treg0,#$addr(error)	; treg0 = error
	}

	codeblock(delayStd,""){
	ldp	#00h
	mar	*,ar5
	lacc	#@delayMask
	samm	dbmr
	lmmr	ar5,#$addr(delayPtr)
	bldd	#$addr(error),*+
	apl	ar5			; the apl/opl thing is there because
	opl	#$addr(delayBuf),ar4	; the C50 doesn't support modulo
	smmr	ar5,#$addr(delayPtr)	; addressing directly.
	sbrk	#$val(errorDelay)	; 0< erroDelay < 256 !!
	apl	ar5
	opl	#$addr(delayBuf),ar5
	lt	*,ar1
	}
	
	codeblock(finalCode,""){
	mpy	#@intMu		; p = 0.5*error*Mu
	ltp	*,ar2		; acc = 0.5*error*Mu
	sach	*,3		; cosOmega = 4*error*Mu
	mpy	*,ar3		; p = 2*error*Mu*x[n-1]
	lacc	*,15		; acc = 0.5*tap
	spac			; acc = 0.5(tap-4*error*Mu*x[n-1])
$starSymbol(skip):
	sach	*,1,ar2		; tap = tap -4*error*Mu*x[n-1]
	neg			; acc = -acc
	sach	*,1,ar1		; cosOmega = -(tap)
	lacc	*-,14		; acc = x[n-2]/4, ar1->x[n-1]
	lt	*		; treg0 = x[n-1]
	dmov	*,ar3		; x[n-2] = x[n-1] <delay>
	mpy	*,ar1		; p = 0.5*tap*x[n-1]
	bldd	#$addr(signalIn),*	; x[n-1] = x[n] <delay>
	apac			; acc = 0.5(x[n-2]/2 + tap*x[n-1])
	add	*,14,ar4	; acc = 0.5(0.5*x[n] +tap*x[n-1] + 0.5*x[n-2])
	sach	*,1
	}


	go {
		CheckParameterValues();

		// Update second tap and compute the output

		addCode(initialCode());
		if (int(errorDelay) > 1) addCode(delayStd());
		else addCode(delayOne);
		addCode(finalCode());
	}

	exectime {
		return time;
	}

}

