defstar {
    name {IIR}
    domain {C50}
    desc {
An infinite impulse response (IIR) filter implemented in a direct form II
realization.  The transfer function is of the form H(z) = G*N(1/z)/D(1/z),
where N() and D() are polynomials.  The parameter "gain" specifies G, and
the fixed-point arrays "numerator" and "denominator" specify N() and D(),
respectively.  Both arrays start with the constant terms of the polynomial
and decrease in powers of z (increase in powers of 1/z).  Note that the
constant term of D is not omitted, as is common in other programs that
assume that it has been normalized to unity.  The denominator coefficients
will be scaled by 1/leading denominator coefficient; the numerator
coefficients will be scaled by gain/leading denominator coefficient.  An
error will result if, after scaling, any of the coefficients is greater
or equal than 1 or less than -1.  Extra code is added to saturate 
accumulator in case of overflow.
    }
    version {@(#)C50IIR.pl	1.16	09/10/99}
    author { Luis Gutierrez, based on the SDF version, G. Arslan}
    copyright {
Copyright (c) 1990-1999 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 dsp library }
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
	type {fix}
    }
    output {
	name {signalOut}
	type {fix}
    }
    defstate {
	name {gain}
	type {int}
	default { "1" }
	desc { Gain. }
    }
    defstate {
	name {numerator}
	type {fixarray}
	default { ".5 .25 .1" }
	desc { Numerator coefficients. }
	attributes { A_CONSTANT|A_SETTABLE|A_UMEM }
    }
    defstate {
	name {denominator}
	type {fixarray}
	default { "1 .5 .3" }
	desc { Denominator coefficients. }
	attributes { A_CONSTANT|A_SETTABLE|A_UMEM }
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

    defstate{
	name {delays}
	type {fixarray}
	default { "0.0" }
	desc { internal state: contains delays }
	attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM }
    }
    defstate{
	name {offset}
	type {int}
	default { 0 }
	desc {internal state}
	attributes { A_NONCONSTANT|A_NONSETTABLE|A_BMEM}
    }
    protected {
	int numState;
	StringList coeffsd, coeffsn;
	int numDenom,numNumer;
    }

    // for max(), sprintf()
    ccinclude { <minmax.h>, <stdio.h> }

    setup {
	numNumer = numerator.size();
	numDenom = denominator.size();
	numState = max(numNumer, numDenom);
	delays.resize(int(numState+1));
	offset.setInitValue(int(numState - 1));
	}

    initCode{
	char	buf[32];
	coeffsd.initialize();
	coeffsn.initialize();

        int i;
	double b0, scaleDenom, scaleNumer;

	// Set up scaling to distribute the gain through the numerator,
	// and scale both numer and denom to make b0 = 1
	if ( numDenom < 1 ) {
	    b0 = 1.0;
	}
	else if ( (b0 = double(denominator[0])) == 0.0 ) {
	    // FIXME: should sanity-check b0 more thoroughly
	    // (e.g., shouldn't even be close to zero)
	    Error::abortRun(*this, "Must have non-zero leading denominator");
	    return;
	}
	if (numDenom > 1)
	scaleDenom = 1.0 / b0;
	scaleNumer = scaleDenom * double(gain);
	delays.resize(numState+1);
	for (i = numState-1; i > 0; i--){
            if ( i < numDenom ) {
                double temp = scaleDenom * -(double(denominator[i]));
                if ((temp >= 1) || (temp < -1)) {
		    StringList msg = "After scaling, denominator coefficient #";
		    msg << i  << " has a value of " << temp
			<< " which is not is the range of [-1, 1).";
                    Error::abortRun(*this, msg);
		    return;
                }
		else {
		    sprintf(buf,"%.15f",temp);
                    coeffsd << buf << " ";
                }
            }
	    else {
                coeffsd << double(0)<<" ";
            }
        }

	denominator.setInitValue(coeffsd);
	
	for (i = 0; i < numState; i++) {
            delays[i] = 0;
            if (i < numNumer) {
                double temp = scaleNumer*double(numerator[i]);
                if ((temp >= 1) || (temp < -1)) {
		    StringList msg = "After scaling, numerator coefficient #";
		    msg << i << " has a value of " << temp
			<< " which is not is the range of [-1, 1).";
                    Error::abortRun(*this, msg);
		    return;
                }
		else {
		    sprintf(buf,"%.15f",temp);
                    coeffsn << buf << " ";
                }
            }
	    else {
		coeffsn << double(0) << " ";
            }
	}
	delays[numState]=double(0);
	numerator.setInitValue(coeffsn);
    }

    go {
	if (numState == 1) {
		addCode(one);
	}
	else {
		addCode(std(int(numState -2),int(numState-1)));
	}
    }

    codeblock(one) {
; H(z) = 1 so just pass input to output
	setc	ovm
	LAR	AR0,#$addr(signalIn)
	LAR	AR1,#$addr(signalOut)
	MAR	*,AR0
	ZAP
	MAC	$addr(numerator),*,AR1
	pac
	sacb
	addb
	clrc	ovm
	SACH	*
    }

    codeblock(std,"int iterD, int iterN") {
;        b[0] + b[1]z^-1 + ... + b[n]z^-n
; H(z) = ----------------------------------
;        1  + a[1]z^-1 + ... + a[n]z^-n
	setc	ovm		; set overflow mode
	lar	ar0,#$addr(delays)
	lar	ar1,#$addr(signalOut)
	lmmr	indx,#$addr(offset)
	mar	*,ar0
	bldd	#$addr(signalIn),*0+
	zap	
	rpt	#@iterD
	macd	$addr(denominator),*-
	apac
	add	*+,15	; acc contains resultA/2
	sacb		; these two inst. are used to
	addb		; saturate acc in case of overflws
	sach	*
	zap
	rpt	#@iterN	
	mac	$addr(numerator),*+
	lta	*,ar1	; acc	contains resultB/2
	sacb		; these two inst. are used to
	addb		; saturate acc in case of overflws
	clrc	ovm
	sach	*	
	}
		

    exectime {
	if (numState == 1) 
		return 10;
	else	
		return (20+2*numState);	
    }
}









