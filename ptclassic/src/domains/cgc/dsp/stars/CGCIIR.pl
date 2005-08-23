defstar {
    name {IIR}
    domain {CGC}
    desc {
An infinite impulse response (IIR) filter implemented in a direct form II
realization.  The transfer function is of the form H(z) = G*N(1/z)/D(1/z),
where N() and D() are polynomials.  The parameter "gain" specifies G, and
the floating-point arrays "numerator" and "denominator" specify N() and D(),
respectively.  Both arrays start with the constant terms of the polynomial
and decrease in powers of z (increase in powers of 1/z).  Note that the
constant term of D is not omitted, as is common in other programs that
assume that it has been normalized to unity.
    }
    version { @(#)CGCIIR.pl	1.9	10/06/96 }
    author { Kennard White, Yu Kee Lim }
    copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CGC library }
	htmldoc {
<p>
This star implements an infinite impulse response filter of arbitrary order
in a direct form II [1] realization.
The parameters of the star specify <i>H</i>(<i>z</i>), the <i>Z</i>-transform of an
impulse response <i>h</i>(<i>n</i>).
The output of the star is the convolution of the input with <i>h</i>(<i>n</i>).
<p>
Note that the numerical finite precision noise increases with the filter order.
To minimize this distortion, it is often desirable to expand the filter
into a parallel or cascade form.
<h3>References</h3>
<p>[1]  
A. V. Oppenheim and R. W. Schafer, <i>Discrete-Time Signal Processing</i>,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
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
	desc { Filter gain }
    }
    defstate {
	name {numerator}
	type {floatarray}
	default { ".5 .25 .1" }
	desc { Numerator coefficients }
    }
    defstate {
	name {denominator}
	type {floatarray}
	default { "1 .5 .3" }
	desc { Denominator coefficients }
    }
    defstate {
	name {state}
	type {floatarray}
	default { "0" }
	desc {State}
	attributes { A_NONCONSTANT|A_NONSETTABLE }
    }
    protected {
	int numState;
    }
    constructor {
	numState = 0;
    }
    // for max()
    ccinclude { <minmax.h> }
    setup {
	int numNumer = numerator.size();
	int numDenom = denominator.size();
	double b0, scaleDenom, scaleNumer;
	numState = max(numNumer,numDenom); 

	// Set up scaling to distribute the gain through the numerator,
	// and scale both numer and denom to make b0=1
	if ( numDenom < 1 ) {
	    b0 = 1.0;
	} else {
	    if ( (b0 = denominator[0]) == 0.0 ) {
		// FIXME: should sanity-check b0 more thoroughly
		// (e.g., shouldn't even be close to zero)
		Error::abortRun(*this, 
		  "Must have non-zero leading coefficient in the denominator");
		return;
	    }
	}
	scaleDenom = 1.0 / b0;
	scaleNumer = scaleDenom * double(gain);

	// Set up the state vector.  The state vector includes
	// both the delay states and the coefficients in the appropriate
	// order:
	// S(0) A(0) -1 S(1) A(1) -B(1) ... S(n-1) A(n-1) -B(n-1) Sn An -Bn
	// state[0] and state[2] are never referenced
	state.resize(numState*3);
	for ( int i=0; i < numState; i++) {
	    state[i*3+0] = 0;
	    state[i*3+1] = i < numNumer ? scaleNumer * numerator[i] : 0;
	    state[i*3+2] = i < numDenom ? scaleDenom * -denominator[i] : 0;
	}
    }
    codeblock(feedThrough) {
	/* No state for the IIR filter: just scale the input */
	$ref(signalOut) = $ref(state,1) * $ref(signalIn);
    }
    codeblock(iir) {
	/*
	   v[0] is the current state variable, v[1] is the current numerator
	   coefficient, and v[2] is the currrent denominator coefficient
	 */
	double* v = & $ref(state,3);
	double* stateEnd = (double *) $ref(state) + $size(state);
	double s0 = $ref(signalIn);
	double  s, y;

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
	$ref(state,3) = s0;
	y += s0 * $ref(state,1);
	$ref(signalOut) = y;
    }
    go {
	if ( numState == 1 ) addCode(feedThrough);
	else addCode(iir);
    }
}
