defstar {
	name { LMSOscDet }
	domain { C50 }

// 	derivedFrom { FIR }
// since 2 of the 3 taps of the filter are 1/2 it's much more convenient
// not to derive this star from C50FIR and use the lacc *,15 instruction
// to implement the multiplication by 1/2

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
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	htmldoc {
The initial taps of this LMS filter are 1, -2, and 1.  The second tap
is adapted while the others are held fixed.  The second tap is equal
to -2<i>a</i><sub>1</sub>, and its adaptation has the form
<pre>
   y<sub> </sub>[n] = x[n] - 2 a<sub>1</sub>[k] x[n-1] + x[n - 2]
   a<sub>1</sub>[k] = a<sub>1</sub>[k-1] + 2 mu e[n] x[n-1]
</pre>
<p>
where <i>y[n]</i> is the output of this filter which can be used as the
error signal.  The step size term <i>mu</i> is fixed by the value of
the <i>stepSize</i> parameter.  You can effectively vary the step size
by attenuating the error term as
<pre>
    e[n] = y[n]/k
</pre>
<p>
assuming that k = 1, 2, 3, and so forth.  When the error becomes relatively
small, this filter gives an estimate of the strongest sinusoidal component:
<pre>
    a<sub>1</sub> = cos(omega)
</pre>
<p>
In this implementation the taps are scaled by 1/2 to make the
star behave like the CG56 version.  Thus the output of the filter is also
scaled by 1/2.  To compensate for this scaling <i>mu</i> is multiplied by 2. 
This filter outputs the current value of <i>a</i><sub>1</sub> on the <i>cosOmega</i>
output port.  The initial value is <i>a</i><sub>1</sub> = 1, that is, zero frequency,
so the initial value of the second tap is -1 (because of the 1/2 scaling).
<p>
For more information on the LMS filter implementation, see the description
of the LMS star upon which this star derived.
<a name="Direct Adaptive Frequency Estimation"></a>
	}
	seealso {FIR, LMS}

	input {
		name { input }
		type { fix }
	}

	output {
		name { output }
		type { fix }
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
		name { delayPtr }
		type { int }
		default { 0 }
		desc {
Pointer to next element to be stored in lmsOldSamples. needed only in delay > 1
		}
		attributes { A_NONSETTABLE|A_NONCONSTANT|A_UMEM }
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
	}

	setup {
		int delay = int(errorDelay);
		if (delay <= 0) Error::abortRun(*this,
				"Error delay must be greater than one!");
		delay++;
		lmsOldSamples.resize(delay);

                // FIXME: Parameters are not always resolved properly
		// before setup but should be.  For now, check parameters
		// in go method.
		// CheckParameterValues();
	
		tap = -cos(double(initialOmega));
		
		double temp = stepSize.asDouble();
		if (temp >= 0) {
			intMu = int(32768*temp + 0.5);
		} else {
			intMu = int(32768*(2+temp) + 0.5);
		}
		
	}

	codeblock(initDelayPtr){
	.ds	$addr(delayPtr)
	.word	$addr(lmsOldSamples)
	.text
	}

	codeblock(initialCode,"") {
	lar	ar4,#$addr(output)
	lar	ar2,#$addr(cosOmega)
	lar	ar3,#$addr(tap)
	}



	codeblock(delayGTOne,""){
	lmmr	ar1,#$addr(delayPtr)
	lmmr	treg0,#$addr(error)	; treg0 = error[n-errorDelay]
	splk	#$addr(lmsOldSamples),cbsr1
	splk	#$addr(lmsOldSamples,@(int(errorDelay))),cber1
	splk	#9h,cbcr		; set circ buffer to be used with one
	mar	*,ar1
	mpy	#@intMu		 ; p = 0.5*error*Mu
	ltp	*		 
; acc = 0.5*error*Mu, treg0 = x[n-errorDelay -1]
	bldd	#$addr(input),*+,ar2	; get new input
	smmr	ar1,#$addr(delayPtr)
	splk	#$addr(lmsOldSamples,@(int(errorDelay))),cbsr1
	splk	#$addr(lmsOldSamples),cber1
	}

	codeblock(delayOfOne,""){
	lar	ar1,#$addr(lmsOldSamples,1)
	lmmr	treg0,#$addr(error)	; treg0 = error
	mar	*,ar1
	mpy	#@intMu		; p = 0.5*error*Mu
	ltp	*,ar2		; acc = 0.5*error*Mu
	}
	
	codeblock(updateTap,""){
	sach	*,3		; cosOmega = 4*error*Mu
	mpy	*,ar3		; p = 2*error*Mu*x[n-2]
	lacc	*,15		; acc = 0.5*tap
	spac			; acc = 0.5(tap-4*error*Mu*x[n-1])
	sach	*,1,ar2		; tap = tap -4*error*Mu*x[n-1]
	neg			; acc = -acc
	sach	*,1,ar1		; cosOmega = -(tap)
	}

	codeblock(firGTOne,""){
	mar	*-		; ar1->x[n]
	lacc	*-,14		; acc = 0.25*x[n]
	lt	*-,ar3		; treg0 = x[n-1]
	mpy	*,ar1		; p = x[n-2]*tap*0.5
	apac			; acc = 0.25*x[n]+0.5*tap*x[n-1]
	add	*,14,ar4	
; acc = 0.25*x[n]+0.5*x[n-1]+0.25*x[n-2]
	sach	*,1		; output = 2*acc
	splk	#0,cbcr		; disable circ buffers 
	}


	codeblock(firOne,""){
	lacc	*-,14		; acc = x[n-2]/4, ar1->x[n-1]
	lt	*		; treg0 = x[n-1]
	dmov	*,ar3		; x[n-2] = x[n-1] <delay>
	mpy	*,ar1		; p = 0.5*tap*x[n-1]
	bldd	#$addr(input),*	; x[n-1] = x[n] <delay>
	apac			; acc = 0.5(x[n-2]/2 + tap*x[n-1])
	add	*,14,ar4	; acc = 0.5(0.5*x[n] +tap*x[n-1] + 0.5*x[n-2])
	sach	*,1
	}


	go {
		CheckParameterValues();

		// Update second tap and compute the output

		addCode(initialCode());
		if (int(errorDelay) > 1) addCode(delayGTOne());
		else addCode(delayOfOne());
		addCode(updateTap());
		if (int(errorDelay) > 1) addCode(firGTOne());
		else addCode(firOne());
	}

	exectime {
		if (int(errorDelay) > 1) return 30;
		else return 23;
	}

}

