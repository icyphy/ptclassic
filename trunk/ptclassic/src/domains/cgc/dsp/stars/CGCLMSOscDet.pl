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
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC library }
	htmldoc {
The initial taps of this LMS filter are 1, -2, and 1.  The second tap
is adapted while the others are held fixed.  The second tap is equal
to <i>-</i>2<i>a</i><sub>1</sub>, and its adaptation has the form
<pre>
y[n] = x[n] - 2a<sub>1[k]</sub>x[n-1] + x[n-2]a<sub>1[k]</sub> = a<sub>1</sub>[k-1] + 2*mu*e[n]x[n-1]
</pre>
<p>
where <i>y[n]</i> is the output of this filter which can be used as the
error signal.  The step size term <i>mu</i> is fixed by the value of
the <i>stepSize</i> parameter.  You can effectively vary the step size
by attenuating the error term as
<pre>
e[n] = y[n] / k
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
scaled by 1/2. 
This filter outputs the current value of <i>a</i><sub>1</sub> on the <i>cosOmega</i>
output port.  The initial value is <i>a</i><sub>1</sub> = 1, that is, zero frequency,
so the initial value of the second tap is -1(because of the 1/2 scaling).
<p>
For more information on the LMS filter implementation, see the description
of the LMS star upon which this star is derived.
<a name="Direct Adaptive Frequency Estimation"></a>
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
		taps[0] =  0.5;
		taps[1] = -1.0 * cos(double(initialOmega));
		taps[2] =  0.5;

		// call the LMS FIR filter setup method
		CGCLMS :: setup();
	}

	codeblock(updateSecondTap,"int index") {
	/* 1. Update the second tap = -a1[k]
	      update:        a1[k] = a1[k]  + 2 mu e[n] x[n-1]
	      second tap:   -a1[k] = -a1[k] - 2 mu e[n] x[n-1]
	      new tap:      newtap = newtap - 2 mu e[n] x[n-1]   */
	double mu = $val(stepSize);
	double e = $ref(error);
	double xnMinus1 = $ref(signalIn,@index);
	double newSecondTap = $ref(taps,1) - 2 * mu * e * xnMinus1;
	$ref(taps,1) = newSecondTap;
	}

	codeblock(outputSecondTap) {
	$ref(cosOmega) = -newSecondTap;
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
		@/* run FIR FILTER */
		@{
		CGCFIR :: go();
		@}
	}
	// Inherit the wrapup method
}



