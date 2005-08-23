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
        version { @(#)SDFIIRFix.pl	1.22	12/08/97 }
        author { E. A. Lee, A. Khazeni, K. White }
        copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
        location { SDF dsp library }
	htmldoc {
<p>
This star implements an infinite impulse response filter of arbitrary order
in a direct form II [1] realization, using fixed-point arithmetic.
The parameters of the star specify <i>H</i>(<i>z</i>), the <i>Z</i>-transform of an
impulse response <i>h</i>(<i>n</i>).
The output of the star is the convolution of the input with <i>h</i>(<i>n</i>).
<p>
Note that the numerical finite precision noise increases with the filter order.
To minimize this distortion, it is often desirable to expand the filter
into a parallel or cascade form.
<p>
Quantization is performed in several places.
First, the coefficients are quantized (rounded) to the precision
specified by <i>CoefPrecision</i>.
Each time the star fires, the
input is optionally quantized (rounded) to precision specified
by <i>InputPrecision</i>.
The multiplication of the state by the coefficients preserves
full precision, but the result is quantized to <i>AccumPrecision</i>
after being added to other products.
The state variables are stored with the precision given by <i>StatePrecision</i>.
The output is quantized (rounded) to <i>OutputPrecision</i> before being
sent to the output.
<p>
The numerator "tap" coefficients and the denominator "feedback"
coefficients have a default precision of 24 bits.
The number of decimal bits is chosen as the minimum number of bits
needed to represent the number in fixed-point.
One bit is reserved for the sign, and the rest are fractional bits.
During computation of filter outputs, the precision of the filter taps
is converted to the precision contained in the "CoefPrecision" parameter.
<h3>References</h3>
<p>[1]  
A. V. Oppenheim and R. W. Schafer, <i>Discrete-Time Signal Processing</i>,
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
		fdbckCoefs = 0;
		fwdCoefs = 0;
		state = 0;
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

	    coef.set_rounding( int(RoundFix) );
	    st.set_rounding( int(RoundFix) );

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
            fixIn.set_rounding( int(RoundFix) );
	    fdbckAccum.set_rounding( int(RoundFix) );
	    fwdAccum.set_rounding( int(RoundFix) );
	    out.set_rounding( int(RoundFix) );
	}
        go {
	    // We use a temporary variable to avoid gcc2.7.2/2.8 problems
	    Fix tmpA = (signalIn%0);
	    int i;
            if ( numState == 1 ) {
                // Actually, this means no state; just feed through
		if ( int(ArrivingPrecision) )
		    out = fwdCoefs[0] * tmpA;
		else {
		    fixIn = tmpA;
		    out = fwdCoefs[0] * fixIn;
		}
		checkOverflow(out);
		signalOut%0 << out;
            } else {
		fdbckAccum = 0.0;
		fwdAccum = 0.0;
		for (i=1; i < numState; i++) {
		    fdbckAccum += state[i] * fdbckCoefs[i];
		    checkOverflow(fdbckAccum);
		    fwdAccum += state[i] * fwdCoefs[i];
		    checkOverflow(fwdAccum);
		}
		if ( int(ArrivingPrecision) )
		    fdbckAccum += tmpA;
		else {
		    fixIn = tmpA;
		    fdbckAccum += (const Fix&)fixIn;
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
