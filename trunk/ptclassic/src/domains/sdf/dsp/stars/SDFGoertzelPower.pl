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
	htmldoc {
<p>
This two-pole, one-zero IIR filter has a transfer function of
<pre>
                  1 + z<sup>-1</sup>
          ------------------------
          1 - 2cos(2 pi/Nk)z<sup>-1</sup> +z<sup>-2</sup>
</pre>
This filter is a biquad filter with
<p>
<ul>
<li> n0 = 1
<li> n1 = 1
<li> n2 = 0
<li> d1 = -2 cos(2 pi k/N)
<li> d2 = 1
</ul>
<p>
This implementation efficiently implements the biquad section based
on the values of these coefficients, and computes d1 from the
parameters k and N.
It is implemented in direct form II, and requires three additions,
one multiply,
<h3>References</h3>
<p>[1]  
A. V. Oppenheim and R. W. Schafer, <i>Discrete-Time Signal Processing</i>,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
	}
	output {
		name{ output }
		type{ float }
	}
	// Inherit setup method
	go {
		// Run the Goertzel second-order IIR filter
		SDFGoertzelBase::go();

		// Compute the power of the kth DFT coefficient.
		// The output of the Goertzel filter is
		//   state1 - Wn*state2 =
		//       state1 - [state2*cos(theta) - j*state2*sin(theta)]
		// where Wn is the twiddle factor Wn = exp(-j theta), such
		// that theta = 2 pi k / N.
		// Power is complex number times its conjugate, z z*:
		//   z = state1 - [state2*cos(theta) - j*state2*sin(theta)]
		//   z = [state1 - state2*cos(theta)] - j [state2*sin(theta)]
		//   z z* = state1*state1 - 2*state1*state2*cos(theta) +
		//     state2*state2*cos^2(theta) + state2*state2*sin^2(theta)
		//   z z* = state1*state1 - state1*state2*2*cos(theta) +
		//          state2*state2
		// where 2*cos(theta) = d1, by definition in SDFGoertzelBase.
		double acc = double(state1)*double(state1) -
			     double(state1)*double(state2)*double(d1) +
		             double(state2)*double(state2);
		output%0 << acc;
	}
}
