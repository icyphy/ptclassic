defstar {
    name {IIR}
    domain {SDF}
    desc {
A Infinite Impulse Response (IIR) filter.
Coefficients are in the "numerator" and "denominator", both start
with z^0 terms and decrease in powers of z.
Default coefficients give an 8th order Butterworth lowpass
filter (XXX: or they will, when I get around to it).
    }
    version {$Id$}
    author { Kennard White }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { SDF dsp library }
    explanation {
.PP
This star implements a infinite-impulse response filter of arbirary order.
The parameters of the star specify H(z), the Z-transform of an
impulse response h(n).  The output of the star is the convolution
of the input with h(n).
The default coefficients give XXX.  The star does not yet have multirate
functionality.
.PP
The transfer function implemented is of the form H(z)=G*N(1/z)/D(1/z),
where N() and D() are polynomials.  The state "gain" specifies G, and
the state arrays "numerator" and "denominator" specify N and D, respectively.
Both arrays start with z^0 terms and decrease in powers of z (increase in
powers of 1/z).  Note that the leading term of D is *not* ommitted.
.PP
Note that the numerical finite precision noise increases with the filter order.
It is often desirable to expand the filter into a parallel or cascade form.
    }
    seealso { FIR, Biquad }
    input {
	name {signalIn}
	type {float}
    }
    output {
	name {signalOut}
	type {float}
    }
    defstate {
	name {gain}
	type {float}
	default { "1" }
	desc { Gain. }
    }
    defstate {
	name {numerator}
	type {floatarray}
	default {
		".5 .25 .1"
	}
	desc { Numerator coefficients. }
    }
    defstate {
	name {denominator}
	type {floatarray}
	default {
		"1 .5 .3"
	}
	desc { Denominator coefficients. }
    }
//    defstate {
//	name {decimation}
//	type {int}
//	default {1}
//	desc {Decimation ratio.}
//   }
//    defstate {
//	name {decimationPhase}
//	type {int}
//	default {0}
//	desc {Downsampler phase.}
//    }
//    defstate {
//	name {interpolation}
//	type {int}
//	default {1}
//	desc {Interpolation ratio.}
//    }
    defstate {
	name {state}
	type {floatarray}
	default { "0" }
	desc {State.}
	attributes { A_NONCONSTANT|A_NONSETTABLE }
    }
    protected {
	int numState;
	double* stateEnd;
    }
    ccinclude {
	<minmax.h>	// for max()
    }
    setup {
	int numNumer = numerator.size();
	int numDenom = denominator.size();
	numState = max(numNumer,numDenom); 
	double b0, scaleDenom, scaleNumer;

	// Set up scaling to distribute the gain through the numerator,
	// and scale both numer and denom to make b0=1
	if ( numDenom < 1 ) {
	    b0 = 1.0;
	} else {
	    if ( (b0 = denominator[0]) == 0.0 ) {
		// XXX: should sanity-check b0 more thoroughly
		// (e.g., shouldnt even be close to zero)
		Error::abortRun(*this, 
		  "Must have non-zero leading denominator");
		return;
	    }
	}
	scaleDenom = 1.0 / b0;
	scaleNumer = scaleDenom * double(gain);

	// Set up the state vector.  The state vector includes
	// both the delay states and the cooeficients in the appropriate
	// order:
	// S(0) A(0) -1 S(1) A(1) -B(1) ... S(n-1) A(n-1) -B(n-1) Sn An -Bn
	// state[0] and state[2] are never referenced
	state.resize(numState*3);
	for ( int i=0; i < numState; i++) {
	    state[i*3+0] = 0;
	    state[i*3+1] = i < numNumer ? scaleNumer * numerator[i] : 0;
	    state[i*3+2] = i < numDenom ? scaleDenom * -denominator[i] : 0;
	}
	stateEnd = ((double*)state) + 3*numState;
    }
    go {
	register double s0 = signalIn%0;

	if ( numState == 1 ) {
	    // actually, this means no state; just feed through
	    signalOut%0 << state[1] * s0;
	} else {
	    register double *v = & state[3];
	    register double y;
	    double s;

	    s = *v++;
	    y = s * *v++;
	    s0 += s * *v++;

	    for ( ; v < stateEnd; ) {
		double sTmp = *v;
		*v++ = s;
		s = sTmp;
		y += s * *v++;
		s0 += s * *v++;
	    }
	    state[3] = s0;
	    y += s0 * state[1];
	    signalOut%0 << y;
	}
    }
}
