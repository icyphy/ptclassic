defstar {
	name { Goertzel }
	domain { CGC }
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
	location { CGC main library }
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
		type { float }
		default { "0.0" }
		desc {
internal state for the storage of the negative of the real part of the
twiddle factor, which is a function of k and N
		}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name { negWnImag }
		type { float }
		default { "0.0" }
		desc {
internal state for the storage of the imaginary part of the twiddle factor,
which is a function of k and N
		}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	setup {
		// check parameter values and compute theta
		CGCGoertzelBase::setup();

		negWnReal = -cos(theta);
		negWnImag = -sin(theta);
	}
	codeblock(finalValue) {
		$ref(output).real = $ref(state1) +
				    $ref(state2) * $ref(negWnReal);
		$ref(output).imag = $ref(state2) * $ref(negWnImag);
	}
	go {
		// Discard all but the last sample
		CGCGoertzelBase::go();

		// Final value is function of the two real IIR state values
		addCode(finalValue);
	}
}
