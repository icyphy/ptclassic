defstar {
	name { Goertzel }
	domain { SDF }
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
	location { SDF dsp library }
	htmldoc {
<p>
This two-pole, one-zero IIR filter has a transfer function of
<pre>
               1 + W<sub>N</sub><sup>k</sup>z<sup>-1</sup>
          ------------------------
          1 - 2cos(2 pi/Nk)z<sup>-1</sup> +z<sup>-2</sup>
</pre>
<p>
This filter is a biquad filter with
<p>
<ul>
<li> n<sub>0</sub> = 1
<li> n<sub>1</sub> = -(W<sub>N</sub>) <sup>k</sup> = exp(j 2 pi k / N)
<li> n<sub>2</sub> = 0
<li> d<sub>1</sub> = -2 cos(2 pi k / N)
<li> d<sub>2</sub> = 1
</ul>
<p>
The implementation takes the simpler forms of n<sub>0</sub>, n<sub>2</sub>,
and d<sub>2</sub>
into account, and computes n<sub>1</sub> and d<sub>1</sub>
from the parameters k and N.
It is implemented in direct form II.
<h3>References</h3>
<p>[1]  
A. V. Oppenheim and R. W. Schafer, <i>Discrete-Time Signal Processing</i>,
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
		SDFGoertzelBase::setup();

		negWnReal = -cos(theta);
		negWnImag = -sin(theta);
	}
	go {
		// Discard all but the last sample
		SDFGoertzelBase::go();

		// Final value is function of the two real IIR state values
		double s1 = state1;
		double s2 = state2;
		Complex o(s1 + s2 * double(negWnReal), s2 * double(negWnImag));
		output%0 << o;
	}
}
