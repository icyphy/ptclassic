defstar {
	name { Goertzel }
	domain { CG56 }
	derived { GoertzelBase }
	version { $Id$ }
	desc {
Second-order recursive computation of the kth coefficient of an
N-point DFT using Goertzel's algorithm.
	}
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 dsp library }
	explanation {
.lp
This two-pole, one-zero IIR filter has a transfer function of
.EQ
{1 ~-~ {W sub N} sup k z sup -1} over
{1 ~-~ 2 cos ( {2 pi} over {N} k ) z sup -1 ~+~ sup -2} ~.
.EN
.ID "Goertzel's algorithm"
This filter is a biquad filter with
.sp 0.5
.(l
n0 = 1
n1 = -Wn = exp(j 2 pi k / N)
n2 = 0
d1 = -2 cos(2 pi k / N)
d2 = 1
.)l
.sp 0.5
The Goertzel's implementation takes the simpler forms of n0, n2, and d2
into account, and computes n1 and d1 from the parameters k and N.
It is implemented in direct form II.
.ID "Oppenheim, A. V."
.ID "Schafer, R. W."
.UH REFERENCES
.ip [1]
A. V. Oppenheim and R. W. Schafer, \fIDiscrete-Time Signal Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
	}
	output {
		name{output}
		type{complex}
	}
	defstate {
		name { negWnReal }
		type { fix }
		default { "0.0" }
		desc {
internal state for the storage of the negative of the real part of the
twiddle factor, which is a function of k and N
		}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name { negWnImag }
		type { fix }
		default { "0.0" }
		desc {
internal state for the storage of the imaginary part of the twiddle factor,
which is a function of k and N
		}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	setup {
		// check parameter values and compute theta
		CG56GoertzelBase::setup();

		negWnReal = -cos(theta);
		negWnImag = -sin(theta);
	}

	codeblock(result) {
; compute complex-valued Goertzel filter output
; a = real part = state1 - state2*cos(theta)
; b = imag part = -state2*sin(theta)
; a already contains the value of state1
		clr	b		#$val(negWnImag),y1
		mac	y1,x1,b		#$val(negWnReal),y0
		mac	x1,y0,a		a,x:$addr(output)
		move	b,y:$addr(output)
	}

	go {
		// Discard all but the last sample
		CG56GoertzelBase::go();

		// Register status after CG56GoertzelBase::go():
		addCode(result);
	}

	exectime {
                // FIXME. Estimates of execution time are given in pairs of
		// oscillator cycles because that's the way it was done in
		// Gabriel: they simply counted the number of instructions.

		return 4 + CG56GoertzelBase::myExecTime();
	}
}
