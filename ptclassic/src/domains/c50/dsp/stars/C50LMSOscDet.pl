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


	constructor {
		// taps state: not constant, length three, and not settable
		taps.clearAttributes(A_CONSTANT);
		taps.clearAttributes(A_SETTABLE);

		// decimation is not supported
		decimation.clearAttributes(A_SETTABLE);
		decimationPhase.clearAttributes(A_SETTABLE);
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

	protected{
		//intMu is a 16 bit representation of stepSize
		int intMu;
	}

	setup {
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
	
		Fix temp("1.15",stepSize.asDouble());
		intMu = int(temp);

		// call the LMS FIR filter setup method
		C50FIR::setup();

	}

	codeblock(oscDetCode,"") {
	setc	ovm			; set overflow mode
	lar	ar1,#$addr(oldSamples,1)	; ar1->x[n-1]
	lar	ar2,#$addr(cosOmega)	; ar2-> cosOmega
	lar	ar3,#$addr(taps,1)	; ar3->taps[1]
	mar	*,ar1
	lmmr	treg0,#$addr(error)	; lmmr = e[n]
	mpy	#@intMu			; preg = 0.5*e[n]*u
	ltp	*,ar2			; treg = x[n-1]; acc = preg
	sach	*,2			; cosOmega = 2*e[n]*u
	mpy	*,ar3			; preg = 0.5(x[n-1]*2*e[n]*u)
	lacc	*,15			; acc = -0.5*secondTap	
	spac				; acc -= preg
	sach	*,1,ar2			; secondTap = 2*acc
	neg
	sach	*,1			; output cosOmega
	}

	go {
		CheckParameterValues();

		// Update second tap and compute the output

		addCode(oscDetCode());

		// Run the FIR filter
		C50FIR :: go();
	}
	// Inherit the wrapup method

	exectime {
		return 15 + C50FIR::myExecTime();
	}

}

