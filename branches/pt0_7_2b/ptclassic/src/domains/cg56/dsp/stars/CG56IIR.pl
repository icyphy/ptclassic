defstar {
    name {IIR}
    domain {CG56}
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
    version { @(#)CG56IIR.pl	1.16	10/06/96 }
    author { Kennard White and Luis Gutierrez}
    copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 dsp library }
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
    defstate {
	name {coeffs}
	type {fixarray}
	default { "0" }
	desc {
internal state: contains coeffs; even addresses correspond to denominator
coeffs., odd addresses correspond to numerator coeffs.
	}
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
    // for max()
    ccinclude { <minmax.h> }

    setup {
	int numNumer = numerator.size();
	int numDenom = denominator.size();
	numState = max(numNumer, numDenom);
	double b0, scaleDenom, scaleNumer;
	numLoops = numState - 1;

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

	scaleDenom = 1.0 / b0;
	scaleNumer = scaleDenom * double(gain);
	coeffs.resize(2*numState);
	delays.resize(numState+1);
	for (int i = 0; i < numState; i++) {
            delays[i] = 0;
            if (i < numNumer) {
                double temp = scaleNumer*double(numerator[i]);
                if ((temp > 1) || (temp < -1)) {
		    StringList msg = "After scaling, numerator coefficient #";
		    msg << i << " has a value of " << temp
			<< " which is not is the range of (-1, 1).";
                    Error::abortRun(*this, msg);
		    return;
                }
		else {
                    coeffs[i*2 + 1] = temp;
                }
            }
	    else {
                coeffs[i*2 + 1] = 0;
            }

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
                    coeffs[i*2] = temp;
                }
            }
	    else {
                coeffs[i*2] = 0;
            }
        }
    }

    go {
	if (numState == 1) {
		addCode(one);
	}
	else {
		addCode(init);
		addCode(doFilter(numLoops));
		addCode(end);
	}
    }

    codeblock(one) {
; H(z) = 1 so just pass input to output
	move	$ref(signalIn),x1
	move	$ref(coeffs,1),x0
	mpyr	x1,x0,a
	move	a,$ref(signalOut)
    }

    codeblock(init) {
;        b[0] + b[1]z^-1 + ... + b[n]z^-n
; H(z) = ----------------------------------
;        1  + a[1]z^-1 + ... + a[n]z^-n
; register after executing this block:
; r0 -> w[1]
; r4 -> a[2]
; x0 == w[1]
; y0 == a[1]
; (a[0] is never referenced since it is normalized in setup)
; a  == sample 
; b  == 0
	move	#($addr(delays)+1),r0
	move	#($addr(coeffs)+2),r4
	move	$ref(signalIn),a
	clr	b	x:(r0),x0	y:(r4)+,y0
    }

    codeblock(doFilter, "int numLoops"){
	do #@numLoops,$label(end_loops)
; x0 == w[p]	y0 == a[p]	x1 == w[p-1]
	mac	y0,x0,a		x1,x:(r0)+	y:(r4)+,y1
; a += w[p]*a[p]	w[p]=w[p-1]	y1 = b[p]
	move	x0,x1						
; x1 = w[p]
	mac	y1,x1,b		x:(r0),x0	y:(r4)+,y0
; b += w[p]*b[p]	x0 = w[p+1] 	y0 = a[p+1]
$label(end_loops)
    }

    codeblock(end) {
; y1 = b[0]
; x0 = sample + a[1]w[1] + ... + a[n]w[n]
; b  = b[0]*{ sample + a[1]w[1] + ... + a[n]w[n]} = output
; w[1] = sample + a[1]w[1] + ... + a[n]w[n]
	move	$ref(coeffs,1),y1
	move	a,x0
	macr	x0,y1,b    a,$ref(delays,1)
	move	b,$ref(signalOut)
    }

    exectime {
	if (numState == 1) return 4;
	return (4 + 3 + 4*numLoops + 4);
    }
}
