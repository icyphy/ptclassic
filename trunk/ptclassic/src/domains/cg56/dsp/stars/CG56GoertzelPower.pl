defstar {
	name { GoertzelPower }
	domain { CG56 }
	derived { GoertzelBase }
	version { $Id$ }
	desc {
Second-order recursive computation of the power of the kth coefficient of
an N-point DFT using Goertzel's algorithm.
This form is used in touchtone decoding.
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
{1 ~+~ z sup -1} over
{1 ~-~ 2 cos ( {2 pi} over {N} k ) z sup -1 ~+~ sup -2} ~.
.EN
.ID "Goertzel's algorithm, simplified form"
This filter is a biquad filter with
.sp 0.5
.(l
n0 = 1
n1 = 1
n2 = 0
d1 = -2 cos(2 pi k / N)
d2 = 1
.)l
.sp 0.5
This implementation efficiently implements the biquad section based
on the values of these coefficients, and computes d1 from the
parameters k and N.
It is implemented in direct form II, and requires three additions,
one multiply,
.ID "Oppenheim, A. V."
.ID "Schafer, R. W."
.UH REFERENCES
.ip [1]
A. V. Oppenheim and R. W. Schafer, \fIDiscrete-Time Signal Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
	}
	output {
		name{ output }
		type{ fix }
	}
	// Inherit setup method
	codeblock(result) {
; compute the power of the last Goertzel filter output z z*, where
; z z* = state1*state1 - state1*state2*2*cos(theta) + state2*state2
		clr	a		b,y1	; a = 0, y1 = b = d1*state2
		mac	x0,x0,a			; a += state1*state1
		mac	x1,x1,a			; a += state2*state2
		mac	-x1,y1,b		; b = -d1*state2*state1
		add	b,a			; a += b
	}
	codeblock(saveResult) {
		move	a,$ref(output)
	}
	go {
		// Run the Goertzel second-order IIR filter
		CG56GoertzelBase::go();

		// Compute the (real-valued) power of last (complex) sample
		addCode(result);
		addCode(saveResult);
	}

	exectime {
		return 5 + CG56GoertzelBase::myExecTime();
	}
}
