defstar {
	name { RaisedCos }
	domain { CG56 }
	derivedFrom { FIR }
	desc {
An FIR filter with a magnitude frequency response shaped
like the standard raised cosine used in digital communications.
See the SDFRaisdCos star for more information.
	}
	version { $Id$ }
	author { J. T. Buck, Kennard White }
	copyright { 1991 The Regents of the University of California }
	location { CG56 dsp library }
	explanation {
See the SDFRaisedCos star.
	}
	seealso {FIR}
	hinclude { <stream.h> }
	code {
		static double rcos (int t, int T, double excess) {
			const double DELTA = 1.0e-7;
			if (t == 0) return 1.0;
			double x = (double)t/(double)T;
			double s = sin (M_PI * x) / (M_PI * x);
			x *= excess;
			double den = 1.0 - 4 * x * x;
			if (den > -DELTA && den < DELTA) return s * M_PI/4.0;
			return s * cos (M_PI * x) / den;
		}
	}
	defstate {
		name { N }
		type { int }
		default { 64 }
		desc { Number of taps }
	}
	defstate {
		name { P }
		type { int }
		default { 16 }
		desc { Distance from center to first zero crossing }
	}
	defstate {
		name { excessBW }
		type { float }
		default { 1.0 }
		desc { Excess bandwidth, between 0 and 1 }
	}
	constructor {
		// taps are no longer constant or settable
		taps.clearAttributes(A_CONSTANT|A_SETTABLE);
		// fix interpolation default
		interpolation.setValue("16");
		// make decimation parameters invisible
		decimation.clearAttributes(A_SETTABLE);

		// XXX: DONT do this...not implemented in CG56FIR
		// XXX: decimationPhase.clearAttributes(A_SETTABLE);
	}
	start {
		taps.resize (N);
		int center = int(N)/2;
		double maxval = double(ONE);
		for (int i = 0; i < int(N); i++) {
			double coef = rcos(i - center, P, excessBW);
			if ( coef > maxval )	coef = maxval;
			if ( coef < -1 )	coef = -1;
			taps[i] = coef;
			// cerr << "Tap %d" << coef << "\n";
		}
		CG56FIR :: start();
	}
}

