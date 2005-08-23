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
than 1 or less than -1.
    }
    version { @(#)C50IIR.pl	1.13	3/13/96 }
    author { Luis Gutierrez, based on the SDF version}
    copyright {
Copyright (c) 1990- The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { C50 dsp library }
    explanation {
.PP
This star implements an infinite impulse response filter of arbitrary order
in a direct form II [1] realization.
The parameters of the star specify $H(z)$, the $Z$-transform of an
impulse response $h(n)$.
The output of the star is the convolution of the input with $h(n)$.
.PP
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
	type {int}
	default { "1" }
	desc { Gain. }
    }
    defstate {
	name {numerator}
	type {fixarray}
	default { ".5 .25 .1" }
	desc { Numerator coefficients. }
    }
    defstate {
	name {denominator}
	type {fixarray}
	default { "1 .5 .3" }
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

    defstate{
	name {delays}
	type {fixarray}
	default { "0.0" }
	desc { internal state: contains delays }
	attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM }
    }
    defstate{
	name {offset}
	type {int}
	default { 0 }
	desc {internal state}
	attributes { A_NONCONSTANT|A_NONSETTABLE|A_UMEM}
    }
    protected {
	int numState;
	StringList coeffs;
	int time;
	int numDenom,numNumer;
    }

    // for max()
    ccinclude { <minmax.h> }

    setup {
	time = 0;
	numNumer = numerator.size();
	numDenom = denominator.size();
	numState = max(numNumer, numDenom);
	delays.resize(int(numState+1));
	offset.setInitValue(int(numState - 1));
	}

    initCode{
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
	if (numDenom > 1) coeffs<<"$starSymbol(dnm):\n";
	scaleDenom = 1.0 / b0;
	scaleNumer = scaleDenom * double(gain);
	delays.resize(numState+1);
	for (int i = numState-1; i > 0; i--){
            if ( i < numDenom ) {
                double temp = scaleDenom * -(double(denominator[i]));
                if ((temp > 1) || (temp < -1)) {
		    StringList msg = "After scaling, denominator coefficient #";
		    msg << i  << " has a value of " << temp
			<< " which is not is the range of (-1, 1).";
                    Error::abortRun(*this, msg);
		    return;
                }
		else {
                    coeffs<<"\t.q15\t"<<temp<<"\n";
                }
            }
	    else {
                coeffs<<"\t.q15\t"<<double(0)<<"\n";
            }
        }

	coeffs<<"$starSymbol(num):\n";
	
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
                    coeffs<<"\t.q15\t"<<temp<<"\n";
                }
            }
	    else {
		coeffs<<"\t.q15\t"<<double(0)<<"\n";
            }
	}
	delays[numState]=double(0);
	coeffs<<"$starSymbol(cfe):\n";
    }

    go {
	if (numState == 1) {
		addCode(one);
		time = 13;
	}
	else {
		addCode(std(int(numState -2),int(numState-1)));
		time = 23 + 2*numState;
	}
	addCode(coeffs);
    }

    codeblock(one) {
; H(z) = 1 so just pass input to output
	LAR	AR0,#$addr(signalIn)
	LAR	AR1,#$addr(signalOut)
	MAR	*,AR0
	ZAP
	MAC	$starSymbol(num),*,AR1
	BCNDD	$starSymbol(cfe),UNC
	SACH	*
	NOP	
    }

    codeblock(std,"int iterD, int iterN") {
;        b[0] + b[1]z^-1 + ... + b[n]z^-n
; H(z) = ----------------------------------
;        1  - a[1]z^-1 - ... - a[n]z^-n
	lar	ar0,#$addr(delays)
	lar	ar1,#$addr(signalOut)
	lmmr	indx,#$addr(offset)
	mar	*,ar0
	bldd	#$addr(signalIn),*0+
	zap	
	rpt	#@iterD
	macd	dnm,*-
	apac
	add	*+,15
	sach	*,1
	zap
	rpt	#@iterN	
	mac	num,*+
	bcndd	cfe,UNC
	lta	*,ar1
	sach	*,1
	}
		

    exectime {
	return time;
    }
}

