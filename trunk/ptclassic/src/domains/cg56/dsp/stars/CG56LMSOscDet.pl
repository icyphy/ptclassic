defstar {
	name { LMSOscDet }
	domain { CG56 }
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
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 dsp library }
	htmldoc {
The initial taps of this LMS filter are 1, -2, and 1.  The second tap
is adapted while the others are held fixed.  The second tap is equal
to <i>-</i>2<i> a <sub></i>1<i></sub></i>, and its adaptation has the form
<pre>
y[n] = x[n] - 2 a<sub>1</sub>[k]x[n-1] + x[n - 2]
a<sub>1</sub>[k] = a<sub>1</sub>[k-1] + 2 mu e[n] x[n-1]
</pre>
where <i>y[n]</i> is the output of this filter which can be used as the
error signal.  The step size term <i>mu</i> is fixed by the value of
the <i>stepSize</i> parameter.  You can effectively vary the step size
by attenuating the error term as
<pre>
e[n] = y[n]
       ----
         k
</pre>
assuming that k = 1, 2, 3, and so forth.  When the error becomes relatively
small, this filter gives an estimate of the strongest sinusoidal component:
<pre>
a<sub>1</sub> = cos(omega)
</pre>
In this implementation the taps of the filter are scaled by 1/2 because
the Motorola 56000 is a fixed point processor and thus the values for the
taps must be in the range [-1,1).
This filter outputs the current value of <i>a <sub></i>1<i></sub></i> on the <i>cosOmega</i>
output port.  The initial value is <i>a <sub></i>1<i></sub> </i>=<i> </i>1, that is, zero frequency,
so the initial value of the second tap is -1(because of the 1/2 scaling).
<p>
For more information on the LMS filter implementation, see the description
of the LMS star upon which this star derived.
<a name="Direct Adaptive Frequency Estimation"></a>
	}
	seealso {FIR, LMS}

	// Since we're not derived from CG56LMS, declare an error port
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
	// FIXME: Remove these states when CG56LMS is derived from CG56FIR
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


	// FIXME: The following two states had to be declare because
	// setSDFParams(arg1,arg2) does not support arg2>arg1 in CG56
	defstate {
		name { pastInputs }
		type { fixarray }
		default { "0.0" }
		desc { internal state used to store past inputs }
		attributes { A_YMEM|A_NONSETTABLE|A_CIRC|A_NONCONSTANT }
	}

	defstate {
		name { nextInput }
		type { int }
		default { 0 }
		desc { 
An internal pointer to the next available position in the 
pastInputs buffer.
		}
		attributes { A_NONSETTABLE|A_YMEM|A_NONCONSTANT }
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


	constructor {
		// taps state: not constant, length three, and not settable
		taps.clearAttributes(A_CONSTANT);
		taps.clearAttributes(A_SETTABLE);

		// decimation is not supported
		decimation.clearAttributes(A_SETTABLE);
		decimationPhase.clearAttributes(A_SETTABLE);
		interpolation.clearAttributes(A_SETTABLE);
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
		if ( int(errorDelay) < 0 ) {
		    Error::abortRun(*this,
				    "The error delay must be non-negative");
		    return;
		}
	    }
	}


	setup {
                // FIXME: Parameters are not always resolved properly
		// before setup but should be.  For now, check parameters
		// in go method.
		// CheckParameterValues();
	
		// we don't support decimation: make sure that it's 1
		decimation = 1;
		decimationPhase = 0;

		// initialize the taps of the three-tap LMS FIR filter
		taps.resize(3);
		taps[0] =  0.5;
		taps[1] = -cos(double(initialOmega));
		taps[2] =  0.5;

		// call the LMS FIR filter setup method
		CG56FIR::setup();
		int bufferSize = int(decimation)*int(errorDelay)
			+ taps.size() + 2;
		input.setSDFParams( int(decimation), int(decimation) - 1);
		pastInputs.resize(bufferSize);

	}

	initCode{
		CG56FIR::initCode();
		addCode(initializeBuffer);
	}

	codeblock(initializeBuffer){
		org		$ref(nextInput)
		dc		$addr(pastInputs)
		org		p:
	}

	codeblock(oscDetCode,"int index, int bufferSize") {
; Compute update of the middle tap, tap[1], according to
; tap[1] = taps[1] - 2 * mu * e * xnMinus1;
; Update the estimate of cos(w) = -tap[1]
;
; Register usage:
; x0 = x[n + (@index + 1)] = input signal delayed by (@index+1) sample(s)
; x = e[n - 1] = error signal delayed by one sample
; y0 = mu = LMS step size
; y1 = -mu e[n - 1] x[n + (@index + 1)]
; a = intermediate computations and the final value of updated tap
; b = initial value of tap to update
	move	$ref(input),x0
	movec	#(@bufferSize-1),m0
	move	$ref(nextInput),r0
	move	#(@index),n0	
	move	x0,y:(r0)+				; save current input in buffer
	move	#$val(stepSize),y0			; y0 = LMS step size
	move	$ref(error),x1				; e[n - 1] error value
	mpyr	x1,y0,a		y:(r0+n0),x0		; x0 = x[n+(@index+1)] input value
	move	a,y0					; x0 = tap value to update
	mpy	-y0,x0,a	$ref(taps,1),b	 
; a = -mu e[n-@index] x[n-@index]
        move    r0,$ref(nextInput)	
; a = -2 mu e[n-@index] x[n-@index]
	addl	b,a				
; a = tap - 2 mu e[n-@index] x[n-@index]
	rnd	a
; round here to decrease effects of rounding.
	move	a,$ref(taps,1)				; save updated tap
	neg	a
	move	a,$ref(cosOmega)			; save cos(w)
	movec	#65535,m0				; restore linear addressing
	}

	go {
		CheckParameterValues();

		// 1. Since we don't support decimation,
		//    index = int(errorDelay) + 1, a constant
		int index = -(int(errorDelay)*int(decimation) + 
			    int(decimationPhase) + 1);

		// 2. Update the middle tap and compute the estimate
		//    of cos(w) = -tap/2
		int bufferSize = CG56LMSOscDet::pastInputs.size();
		addCode(oscDetCode(index, bufferSize));

		// 3. Run the FIR filter
		CG56FIR :: go();
	}
	// Inherit the wrapup method

	exectime {
		return 17 + CG56FIR::myExecTime();
	}

}

