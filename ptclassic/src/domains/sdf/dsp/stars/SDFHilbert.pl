defstar {
	name { Hilbert }
	domain { SDF }
	derivedFrom { FIR }
	desc {
Output the (approximate) Hilbert transform of the input signal.
This star approximates the Hilbert transform by using an FIR filter.
	}
	version { $Id$ }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
.Id "Hilbert filter"
.Id "filter, Hilbert"
The exact Hilbert transform cannot be realized; instead, we just truncate
the response symmetrically at $-N/2$ and $N/2$ [1].
This is accurate enough for some applications, but for high accuracy we
suggest the use of the Parks-McClellan algorithm [2] to design a Hilbert
transformer filter with the desired characteristics [1,3].
The "optfir" program supplied with Ptolemy can do this.
.Ir "optfir program"
.ID "Oppenheim, A. V."
.ID "Rabiner, L. R."
.ID "Schafer, R. W."
.ID "Parks, T. W."
.UH REFERENCES
.ip [1]
A. V. Oppenheim and R. W. Schafer, \fIDiscrete-Time Signal Processing\fR,
Prentice-Hall: Englewood Cliffs, NJ, 1989.
.ip [2]
T. W. Parks and J. H. McClellan,
``Chebyshev Approximation for Nonrecursive Digital Filters With Linear Phase,''
\fIIEEE Trans. on Circuit Theory\fR, vol. 19, no. 2, pp. 189-194, March 1972.
.ip [3]
L. R. Rabiner, J. H. McClellan, and T. W. Parks,
``FIR Digital Filter Design Techniques Using Weighted Chebyshev Approximation,''
\fIProc. of the IEEE\fR, vol. 63, no. 4, pp. 595-610, April 1975.
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
