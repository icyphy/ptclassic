defstar {
        name {IIRFix}
        domain {SDF}
	derivedFrom { SDFFix }
        desc {
An infinite impulse response (IIR) filter implemented in direct form II,
using fixed-point arithmetic.  The transfer function is of the form
H(z) = G*N(1/z)/D(1/z), where N() and D() are polynomials.
The parameter "gain" specifies G, and the floating-point arrays
"numerator" and "denominator" specify N() and D(), respectively.
Both arrays start with the constant terms of the polynomial
and decrease in powers of z (increase in powers of 1/z).
The coefficients are rounded to the precision given by "CoefPrecision".
Before the numerator and denominator coefficients are quantized, they
are rescaled so that the leading denominator coefficient is unity.
The gain is multiplied through the numerator coefficients as well.
        }
        version { $Id$ }
        author { E. A. Lee, A. Khazeni, K. White }
        copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        location { SDF dsp library }
        explanation {
.pp
This star implements an infinite impulse response filter of arbitrary order
in a direct form II [1] realization, using fixed-point arithmetic.
The parameters of the star specify $H(z)$, the $Z$-transform of an
impulse response $h(n)$.
The output of the star is the convolution of the input with $h(n)$.
.pp
Note that the numerical finite precision noise increases with the filter order.
To minimize this distortion, it is often desirable to expand the filter
into a parallel or cascade form.
.pp
Quantization is performed in several places.
First, the coefficients are quantized (rounded) to the precision
specified by \fICoefPrecision\fR.
Each time the star fires, the
input is optionally quantized (rounded) to precision specified
by \fIInputPrecision\fR.
The multiplication of the state by the coefficients preserves
full precision, but the result is quantized to \fIAccumPrecision\fR
after being added to other products.
The state variables are stored with the precision given by \fIStatePrecision\fR.
The output is quantized (rounded) to \fIOutputPrecision\fR before being
sent to the output.
.pp
The numerator "tap" coefficients and the denominator "feedback"
coefficients have a default precision of 24 bits.
The number of decimal bits is chosen as the minimum number of bits
needed to represent the number in fixed-point.
One bit is reserved for the sign, and the rest are fractional bits.
During computation of filter outputs, the precision of the filter taps
is converted to the precision contained in the "CoefPrecision" parameter.
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
                type { precision }
                default { 2.14 }
                desc { Precision of the coefficients in bits. }
        }
	defstate {
		name { ArrivingPrecision }
		type {int}
		default {"YES"}
		desc {
Flag indicating whether or not to use the arriving particles as they are:
YES keeps the same precision, and NO casts them to the precision specified
by the parameter "InputPrecision".
		}
	}
        defstate {
                name { InputPrecision }
                type { precision }
                default { 2.14 }
                desc {
Precision of the input in bits.
The input particles are only cast to this precision if the parameter
"ArrivingPrecision" is set to NO.
		}
        }
        defstate {
                name { AccumPrecision }
                type { precision }
                default { 2.14 }
                desc { Precision of the accumulation in bits. }
        }
        defstate {
                name { StatePrecision }
                type { precision }
                default { 2.14 }
                desc { Precision of the state in bits. }
        }
        defstate {
                name { OutputPrecision }
                type { precision }
                default { 2.14 }
                desc { Precision of the output in bits. } 
        }
	ccinclude {
		<minmax.h>      // for max()
	}
        protected {
		int numState;
		Fix *fdbckCoefs, *fwdCoefs, *state;
		Fix fixIn, fdbckAccum, fwdAccum, out;
        }
	constructor {
		fdbckCoefs = fwdCoefs = state = NULL;
	}
	destructor {
	    LOG_DEL; delete [] fdbckCoefs;
	    LOG_DEL; delete [] fwdCoefs;
	    LOG_DEL; delete [] state;
	}
        setup {
            SDFFix::setup();

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

	    // Local variables
	    Fix coef( (const char*) CoefPrecision );
	    if ( coef.invalid() )
	      Error::abortRun( *this, "Invalid CoefPrecision" );

	    Fix st( (const char*) StatePrecision );
	    if ( st.invalid() )
	      Error::abortRun( *this, "Invalid StatePrecision" );

	    coef.set_rounding( ((int) RoundFix) );
	    st.set_rounding( ((int) RoundFix) );

	    // Set up the state and coefficient vectors
	    // The first location in state and fdbckCoefs is wasted.
	    for ( int i=0; i < numState; i++) {
		st = 0.0;
	        state[i] = st;
	        coef = i<numNumer ? scaleNumer * numerator[i] : 0.0;
	        fwdCoefs[i] = coef;
	        coef = i<numDenom ? scaleDenom * -denominator[i] : 0.0;
	        fdbckCoefs[i] = coef;
	    }

	    // Set the precision of various fixed-point variables
	    fixIn = Fix( (const char*) InputPrecision );
	    if ( fixIn.invalid() )
	      Error::abortRun( *this, "Invalid InputPrecision" );

	    fdbckAccum = Fix ( (const char*) AccumPrecision );
	    if ( fdbckAccum.invalid() )
	      Error::abortRun( *this, "Invalid AccumPrecision" );
	    fwdAccum = Fix ( (const char*) AccumPrecision );

	    out = Fix ( (const char*) OutputPrecision );
	    if ( out.invalid() )
	      Error::abortRun( *this, "Invalid OutputPrecision" );

	    // Set the overflow handlers for assignments/computations
	    fixIn.set_ovflow( OverflowHandler );
	    if ( fixIn.invalid() )
	      Error::abortRun( *this, "Invalid OverflowHandler" );
	    fdbckAccum.set_ovflow( OverflowHandler );
	    fwdAccum.set_ovflow( OverflowHandler );
	    out.set_ovflow( OverflowHandler );

	    // Set all fixed-point assignments/computations to use rounding
            fixIn.set_rounding( ((int) RoundFix) );
	    fdbckAccum.set_rounding( ((int) RoundFix) );
	    fwdAccum.set_rounding( ((int) RoundFix) );
	    out.set_rounding( ((int) RoundFix) );
	}
        go {
            if ( numState == 1 ) {
                // Actually, this means no state; just feed through
		if ( int(ArrivingPrecision) )
		    out = fwdCoefs[0] * Fix(signalIn%0);
		else {
		    fixIn = Fix(signalIn%0);
		    out = fwdCoefs[0] * fixIn;
		}
		checkOverflow(out);
		signalOut%0 << out;
            } else {
		fdbckAccum = 0.0;
		fwdAccum = 0.0;
		for ( int i=1; i < numState; i++) {
		    fdbckAccum += state[i] * fdbckCoefs[i];
		    checkOverflow(fdbckAccum);
		    fwdAccum += state[i] * fwdCoefs[i];
		    checkOverflow(fwdAccum);
		}
		if ( int(ArrivingPrecision) )
		    fdbckAccum += Fix(signalIn%0);
		else {
		    fixIn = Fix(signalIn%0);
		    fdbckAccum += fixIn;
		}
		checkOverflow(fdbckAccum);
		for ( i=numState-1; i > 1; i--) {
		    state[i] = state[i-1];
		}
		state[1] = fdbckAccum;
		out = fdbckAccum * fwdCoefs[0] + fwdAccum;
		checkOverflow(out);
		signalOut%0 << out;
	     }
	}
        // A wrap-up method is inherited from SDFFix
        // If you defined your own, you should call SDFFix::wrapup()
}
