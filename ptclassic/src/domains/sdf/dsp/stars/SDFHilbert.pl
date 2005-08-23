defstar {
	name { Hilbert }
	domain { SDF }
	derivedFrom { FIR }
	desc {
Output the (approximate) Hilbert transform of the input signal.
This star approximates the Hilbert transform by using an FIR filter.
	}
	version { @(#)SDFHilbert.pl	1.15 10/06/96 }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	htmldoc {
<a name="Hilbert filter"></a>
<a name="filter, Hilbert"></a>
The exact Hilbert transform cannot be realized; instead, we just truncate
the response symmetrically at <i>-N/</i>2 and <i>N/</i>2 [1].
This is accurate enough for some applications, but for high accuracy we
suggest the use of the Parks-McClellan algorithm [2] to design a Hilbert
transformer filter with the desired characteristics [1,3].
The "optfir" program supplied with Ptolemy can do this.
<a name="optfir program"></a>
<h3>References</h3>
<p>[1]  
A. V. Oppenheim and R. W. Schafer, <i>Discrete-Time Signal Processing</i>,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
<p>[2]  
T. W. Parks and J. H. McClellan,
``Chebyshev Approximation for Nonrecursive Digital Filters With Linear Phase,''
<i>IEEE Trans. on Circuit Theory</i>, vol. 19, no. 2, pp. 189-194, March 1972.
<p>[3]  
L. R. Rabiner, J. H. McClellan, and T. W. Parks,
``FIR Digital Filter Design Techniques Using Weighted Chebyshev Approximation,''
<i>Proc. of the IEEE</i>, vol. 63, no. 4, pp. 595-610, April 1975.
	}
	ccinclude { <math.h> }
	defstate {
		name { N }
		type { int }
		default { 64 }
		desc { Number of taps }
	}
	constructor {
		// taps are no longer constant or settable
		taps.clearAttributes(A_CONSTANT|A_SETTABLE);
	}
	setup {
		int len = int(N);
		int len2 = len / 2;
		int i, n;
		double k = 2.0 / M_PI;

		taps.resize(len);
		for (i = 0, n = -len2; i < len; i++, n++) {
			if ((n & 1) != 0)
				taps[i] = k / n;
			else
				taps[i] = 0.0;
		}
		SDFFIR :: setup();
	}
}
