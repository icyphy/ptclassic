defstar {
	name { GoertzelPower }
	domain { SDF }
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
	location { SDF dsp library }
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
		type{ float }
	}
	// Inherit setup method
	go {
		SDFGoertzelBase::go();

		// For the last sample, compute the power.  The output is
		//   state1 - Wn*state2 =
		//       state1 - [state2*cos(theta) + j*state2*sin(theta)]
		// where Wn is the twiddle factor Wn = exp(-j theta), such
		// that theta = 2 pi k / N.  Magnitude squared of the output:
		//   [state1 - state2*cos(-theta)]^2 + [state2*sin(-theta)]^2 =
		//       state1 + [-2*cos(theta)]*state1*state2 + state2^2
		// which is a quadratic in state2, so use Horner's form:
		//   state1 + state2 * ( d1val * state1 + state2 )
		double acc = double(state2);
		acc += double(d1) * double(state1);
		acc *= double(state2);
		acc += double(state1);
		output%0 << acc;
	}
}
