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
		for (i = 0, n = -len2; i < len; i++, n++)
			taps[i] = (n & 1) ? k / n : 0.0;
		SDFFIR :: start();
	}
}
