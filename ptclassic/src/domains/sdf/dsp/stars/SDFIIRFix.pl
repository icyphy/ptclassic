defstar {
        name {IIRFix}
        domain {SDF}
        desc {
An infinite impulse response (IIR) filter implemented in direct form II,
using fixed-point arithmetic.
The transfer function is of the form H(z) = G*N(1/z)/D(1/z),
where N() and D() are polynomials.
The parameter "gain" specifies G, and the floating-point arrays
"numerator" and "denominator" specify N() and D(), respectively.
Both arrays start with the constant terms of the polynomial
and decrease in powers of z (increase in powers of 1/z).
Note that the constant term of D is not omitted, as is common in
other programs that assume that it has been normalized to unity.
Before the numerator and denominator coefficients are quantized, they
are rescaled so that the leading denominator coefficient is unity.
The gain is multiplied through the numerator coefficients as well.
        }
        version { $Id$ }
        author { E. A. Lee, A. Khazeni, K. White }
        copyright {
Copyright (c) 1994 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        location { SDF dsp library }
        explanation {
.pp
This star implements an infinite impulse response filter of arbitrary order
in a direct form II [1] realization, using fixed point arithmetic.
The parameters of the star specify $H(z)$, the $Z$-transform of an
impulse response $h(n)$.
The output of the star is the convolution of the input with $h(n)$.
.pp
Note that the numerical finite precision noise increases with the filter order.
To minimize this distortion, it is often desirable to expand the filter
into a parallel or cascade form.
.ID "Schafer, R. W."
.ID "Oppenheim, A. V."
.UH REFERENCES
.ip [1]
A. V. Oppenheim and R. W. Schafer, \fIDiscrete-Time Signal Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
        }
        seealso { FIR, Biquad }
        input {
                name {signalIn}
                type {fix}
        }
        output {
                name {signalOut}
                type {fix}
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
		default { ".5 .25 .1" }
		desc { Numerator coefficients. }
	}
	defstate {
		name {denominator}
		type {floatarray}
		default { "1 .5 .3" }
		desc { Denominator coefficients. }
	}
        defstate {
                name { CoefPrecision }
                type { string }
                default { "2.14" }
                desc { Precision of the coefficients in bits. }
        }
        defstate {
                name { InputPrecision }
                type { string }
                default { "2.14" }
                desc { Precision of the input in bits.  }
        }
        defstate {
                name { AccumulationPrecision }
                type { string }
                default { "2.14" }
                desc { Precision of the accumulation and state in bits. }
        }
        defstate {
                name { OutputPrecision }
                type { string }
                default { "2.14" }
                desc { Precision of the output in bits. } 
        }
	ccinclude {
		<minmax.h>      // for max()
	}
        protected {
                int In_len;
                int In_intBits;
                int Coef_len;
                int Coef_intBits;
                int Accum_len;
                int Accum_intBits;
                int Out_len;
                int Out_intBits;
		int numState;

		Fix *fdbckCoefs, *fwdCoefs, *state;
        }
	constructor {
		fdbckCoefs = fwdCoefs = state = NULL;
	}
        begin {
	    const char* IP = InputPrecision;
	    const char* TP = CoefPrecision;
	    const char* AP = AccumulationPrecision;
	    const char* OP = OutputPrecision;
	    In_len = Fix::get_length (IP);
	    In_intBits = Fix::get_intBits (IP);
	    Coef_len = Fix::get_length (TP);
	    Coef_intBits = Fix::get_intBits (TP);
	    Accum_len = Fix::get_length (AP);
	    Accum_intBits = Fix::get_intBits (AP);
	    Out_len = Fix::get_length (OP);
	    Out_intBits = Fix::get_intBits (OP);

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
		    // (e.g., shouldn't even be close to zero)
		    Error::abortRun(*this,
		      "Must have non-zero leading denominator coefficient");
		    return;
	        }
	    }
	    scaleDenom = 1.0 / b0;
	    scaleNumer = scaleDenom * double(gain);

	    LOG_DEL; delete [] fdbckCoefs;
	    LOG_NEW; fdbckCoefs = new Fix[numState];
	    LOG_DEL; delete [] fwdCoefs;
	    LOG_NEW; fwdCoefs = new Fix[numState];
	    LOG_DEL; delete [] state;
	    LOG_NEW; state = new Fix[numState];
    
	    Fix coef(Coef_len, Coef_intBits);
	    Fix st(Accum_len, Accum_intBits);

	    // Set up the state and coefficient vectors
	    // The first location in state and fdbckCoefs is wasted.
	    for ( int i=0; i < numState; i++) {
		st = 0.0;
	        state[i] = st;
	        coef = i<numNumer ? scaleNumer * numerator[i] : 0;
	        fwdCoefs[i] = coef;
	        coef = i<numDenom ? scaleDenom * -denominator[i] : 0;
	        fdbckCoefs[i] = coef;
	    }
	}
        go {
	    Fix fdbckAccum(Accum_len, Accum_intBits),
	    	fwdAccum(Accum_len, Accum_intBits),
		fixIn(In_len, In_intBits),
	    	out(Out_len, Out_intBits);

            if ( numState == 1 ) {
                // actually, this means no state; just feed through
		fixIn = Fix(signalIn%0);
		out = fwdCoefs[0] * fixIn;
		signalOut%0 << out;
            } else {
		fdbckAccum = 0.0;
		fwdAccum = 0.0;
		for ( int i=1; i < numState; i++) {
		    fdbckAccum = fdbckAccum + state[i] * fdbckCoefs[i];
		    fwdAccum = fwdAccum + state[i] * fwdCoefs[i];
		}
		fixIn = Fix(signalIn%0);
		fdbckAccum = fdbckAccum + fixIn;
		for ( i=numState-1; i > 1; i--) {
		    state[i] = state[i-1];
		}
		state[1] = fdbckAccum;
		out = fdbckAccum * fwdCoefs[0] + fwdAccum;
		signalOut%0 << out;
	     }
	 }
}
