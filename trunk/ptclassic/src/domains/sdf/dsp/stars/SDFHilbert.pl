defstar {
	name { Hilbert }
	domain { SDF }
	derivedFrom { FIR }
	desc {
Output the (approximate) Hilbert transform of the input signal.
	}
	version { $Id$ }
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF dsp library }
	explanation {
This star approximates the Hilbert transform by using an FIR filter.
.Id "Hilbert filter"
.Id "filter, Hilbert"
The exact Hilbert transform cannot be realized; instead, we just chop
off the response symmetrically at -N/2 and N/2.  This is good enough
for demos, but for high accuracy we suggest the use of the Parks-McClellan
algorithm to design a Hilbert transformer filter with the desired
characteristics.  The "optfir" program supplied with Ptolemy can do this.
.Ir "optfir program"
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
	start {
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
		SDFFIR :: start();
	}
}
