defstar {
    name {IIR}
    domain {CG56}
    desc {
An infinite impulse response (IIR) filter implemented in a direct form II
realization.
The transfer function is of the form H(z) = G*N(1/z)/D(1/z),
where N() and D() are polynomials.
The parameter "gain" specifies G, and the floating-point arrays
"numerator" and "denominator" specify N() and D(), respectively.
Both arrays start with the constant terms of the polynomial
and decrease in powers of z (increase in powers of 1/z).
Note that the constant term of D is not omitted, as is common in
other programs that assume that it has been normalized to unity.
    }
    version { $Id$ }
    author { Kennard White & Luis Gutierrez}
    copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 dsp library }
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
	default {
		".5 .25 .1"
	}
	desc { Numerator coefficients. }
    }
    defstate {
	name {denominator}
	type {fixarray}
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
	name {coeffs}
	type {fixarray}
	default { "0" }
	desc {internal state: contains coeffs}
	attributes { A_NONCONSTANT|A_NONSETTABLE|A_YMEM }
    }
    defstate{
	name {delays}
	type {fixarray}
	default { "0" }
	desc { internal state: contains delays }
	attributes { A_NONCONSTANT|A_NONSETTABLE|A_XMEM }
   }
    protected {
	int numState, numLoops;
    }
    ccinclude {
	<minmax.h>	// for max()
    }
    setup {
	int numNumer = numerator.size();
	int numDenom = denominator.size();
	numState = max(numNumer,numDenom);
	double b0, scaleDenom, scaleNumer;
	numLoops = numState - 1;
	// Set up scaling to distribute the gain through the numerator,
	// and scale both numer and denom to make b0=1
	if ( numDenom < 1 ) {
	    b0 = 1.0;
	} else {
	    if ( (b0 = double(denominator[0])) == 0.0 ) {
		// XXX: should sanity-check b0 more thoroughly
		// (e.g., shouldn't even be close to zero)
		Error::abortRun(*this, 
		  "Must have non-zero leading denominator");
		return;
	    }
	}
	scaleDenom = 1.0 / b0;
	scaleNumer = scaleDenom * double(gain);
	coeffs.resize(2*numState);
	delays.resize(numState+1);
	for ( int i=0; i < numState; i++) {
	    delays[i]    = 0;
	    coeffs[i*2+1]       = i < numNumer ? scaleNumer * double(numerator[i]) : 0;
	    coeffs[i*2]         = i < numDenom ? scaleDenom * -(double(denominator[i])) : 0;
	}

    }
    go{
	if (numState == 1) {
		addCode(one);
	} else {
		addCode(init);
		addCode(do_filter(numLoops));
		addCode(end);
	}
    }
    codeblock(one){
		move $ref(signalIn),x1
		move $ref(coeffs,1),x0
		mpyr x1,x0,a
		move a,$ref(signalOut)
    }
    codeblock(init){
		move #($addr(delays)+1),r0
		move #($addr(coeffs)+2),r4
		move $ref(signalIn),a
		clr b   x:(r0),x0     y:(r4)+,y0
    }
    codeblock(do_filter, "int numLoops"){
		do #@numLoops,$label(end_loops)
		mac y0,x0,a  x1,x:(r0)+      y:(r4)+,y1
		move x0,x1                   ; delay 
		mac y1,x1,b  x:(r0),x0       y:(r4)+,y0
$label(end_loops)
    }
    codeblock(end){
		move $ref(coeffs,1),y1
		move a,x0
		macr x0,y1,b    a,$ref(delays,1)
		move b,$ref(signalOut)
    }
    exectime{
	if (numState == 1) {
		return 4;
	} else {
		return (4+ 3 + 4*numLoops + 4 );
        }
   }		

}
