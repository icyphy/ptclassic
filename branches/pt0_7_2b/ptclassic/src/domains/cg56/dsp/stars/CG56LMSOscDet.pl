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
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 dsp library }
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

	// FIXME: Remove the error port when CG56LMS is derived from CG56FIR
        input {
		name { error }
		type { float }
	}

	// Unique port for LMSOscDet
	output {
		name { cosOmega }
		type { float }
		desc {
Current estimated value of the cosine of the frequency of the dominate
sinusoidal component of the input signal.
		}
	}

	// FIXME: Remove these states when CG56LMS is derived from CG56FIR
        defstate {
                name { stepSize }
                type { float }
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
	state {
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

		// FIXME: Add the following lines back in when CG56FIR
		// is updated to match SDFFIR
		// decimationPhase.clearAttributes(A_SETTABLE);
	}
	setup {
		if ( double(stepSize) < 0.0 ) {
		    Error::abortRun(*this,
				    "The step size must be positive");
		    return;
		}

		// we don't support decimation: make sure that it's 1
		decimation = 1;

		// initialize the taps of the three-tap LMS FIR filter
		taps.resize(3);
		taps[0] =  1.0;
		taps[1] = -2.0 * cos(double(initialOmega));
		taps[2] =  1.0;

		// call the LMS FIR filter setup method
		CG56FIR :: setup();
	}
	codeblock(oscDetCode,"int index") {
; Compute update of the middle tap, tap[1], according to
; tap[1] = taps[1] - 4 * mu * e * xnMinus1;
; Update the estimate of cos(w) = -tap[1]/2
;
; Register usage:
; x0 = x[n - @index] = input signal delayed by @index sample(s)
; x1 = e[n - @index] = error signal delayed by one sample
; y0 = mu = LMS step size
; y1 = -mu e[n - @index] x[n - @index]
; a = intermediate computations and the final value of updated tap
; b = initial value of tap to update
	move	#$val(stepSize),y0		; y1 = mu = LMS step size
	move	$ref(error),x1		; e[n - @index] error value
	mpyr	x1,y0,a		$ref(input,@index),x0	; x0 = x[n-@index] input value
	move	a,y0	y:$addr(taps)+1,b		; x0 = tap value to update
	mpyr	-y0,x0,a		; a = -mu e[n-@index] x[n-@index]
	asl	a		; a = -2 mu e[n-@index] x[n-@index]
	addl	b,a		; a = tap - 4 mu e[n-@index] x[n-@index]
	move	a,y:$addr(taps)+1		; save updated tap
	asr	a
	neg	a
	move	a,$ref(cosOmega)		; save estimate of cos(w) = -tap/2
	}
	go {
		// 1. Since we don't support decimation,
		//    index = int(errorDelay) + 1, a constant
		int index = int(errorDelay)*int(decimation) + 1;
		// + int(decimationPhase);

		// 2. Update the middle tap and compute the estimate
		//    of cos(w) = -tap/2
		addCode(oscDetCode(index));

		// 3. Run the FIR filter
		CG56FIR :: go();
	}
	// Inherit the wrapup method
	exectime {
		return 11 + CG56FIR::myExecTime();
	}
}
