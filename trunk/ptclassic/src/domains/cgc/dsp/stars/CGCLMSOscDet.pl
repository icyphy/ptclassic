defstar {
	name { LMSOscDet }
	domain { CGC }
	derivedFrom { LMS }
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
Copyright (c) 1990- The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC library }
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
	output {
		name { cosOmega }
		type { float }
		desc {
Current estimated value of the cosine of the frequency of the dominate
sinusoidal component of the input signal.
		}
	}
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
		decimationPhase.clearAttributes(A_SETTABLE);
	}
	setup {
		// we don't support decimation: make sure that it's 1
		decimation = 1;

		// initialize the taps of the three-tap LMS FIR filter
		taps.resize(3);
		taps[0] =  1.0;
		taps[1] = -2.0 * cos(double(initialOmega));
		taps[2] =  1.0;

		// call the LMS FIR filter setup method
		CGCLMS :: setup();
	}

	codeblock(updateSecondTap,"int index") {
	/* 1. Update the second tap = -2 a1[k]
	      update:        a1[k] = a1[k] + 2 e[n] x[n-1]
	      second tap: -2 a1[k] = -2 a1[k] - 4 mu e[n] x[n-1]
	      new tap:      newtap = newtap - 4 mu e[n] x[n-1]   */
	double mu = $val(stepSize);
	double e = $ref(error);
	double xnMinus1 = $ref(signalIn,@index);
	double newSecondTap = $ref(taps,1) - 4 * mu * e * xnMinus1;
	$ref(taps,1) = newSecondTap;
	}

	codeblock(outputSecondTap) {
	$ref(cosOmega) = -newSecondTap/2;
	}

	go {
		// 1. Update the second tap
		//    Since we don't support decimation,
		//    index = int(errorDelay) + 1, a constant
		int index = int(errorDelay)*int(decimation) +
			    int(decimationPhase) + 1;
		addCode(updateSecondTap(index));

		// 2. Compute the estimate of cos(w), a1
		addCode(outputSecondTap);

		// 3. Run the FIR filter
		CGCFIR :: go();
	}
	// Inherit the wrapup method
}
