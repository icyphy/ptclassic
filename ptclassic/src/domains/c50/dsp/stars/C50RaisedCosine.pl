defstar {
	name { RaisedCos }
	domain { C50 }
	derivedFrom { FIR }
	desc {
An FIR filter with a magnitude frequency response that is shaped
like the standard raised cosine or square-root raised cosine
used in digital communications.
	}
	version { $Id$ }
	author { A.Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 dsp library }
	explanation {
See the SDFRaisedCos star.
	}
	seealso { FIR, Window }
	// <math.h> is already included to define M_PI and the sin function
	code {
		// The code is written with integer arguments so that it
		// can reliably detect the 0/0 condition, avoiding
		// roundoff problems.
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
		static double sqrtrcos (int t, int T, double excess) {
		    double sqrtT = sqrt(double(T));
		    if (t == 0) {
			return ((4*excess/M_PI) + 1 - excess)/sqrtT;
		    }
		    double x = double(t)/double(T);
		    if (excess == 0.0) {
			return sqrtT*sin( M_PI * x)/(M_PI * t);
		    }
		    double oneplus = (1.0+excess)*M_PI/T;
		    double oneminus = (1.0-excess)*M_PI/T;
		    // Check to see whether we will get divide by zero
		    double den = t*t*16*excess*excess - T*T;
		    if (den == 0) {
		        return (T*sqrtT/(8 * excess * M_PI * t))
			    * ( oneplus * sin( oneplus * t)
			    - (oneminus*T/(4*excess*t)) * cos( oneminus * t )
			    + (T/(4*excess*t*t)) * sin ( oneminus * t) );
		    }
		    return (4*excess/(M_PI*sqrtT))
			* (cos(oneplus*t) + sin(oneminus*t)/(x*4*excess))
			/ (1.0 - 16*excess*excess*x*x);
		}
	}
	defstate {
		name { length }
		type { int }
		default { 64 }
		desc { Number of taps }
	}
	defstate {
		name { symbol_interval }
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
	defstate {
		name { square_root }
		type { int }
		default { NO }
		desc { If YES, use square-root raised cosine pulse }
	}
	constructor {
		// taps are no longer constant or settable
		taps.clearAttributes(A_CONSTANT|A_SETTABLE);
		// fix interpolation default
		// interpolation.setInitValue("16");
	}
	setup {
		if (double(excessBW) < 0.0)
		    Error::abortRun(*this, "Invalid excess bandwidth");
		if (int(symbol_interval) <= 0)
		    Error::abortRun(*this, "Invalid symbol interval");
		taps.resize(length);
		int center = int(length)/2;
		for (int i = 0; i < int(length); i++)
		    if (int(square_root))
			taps[i] = sqrtrcos(i - center, symbol_interval, excessBW);
		    else
			taps[i] = rcos (i - center, symbol_interval, excessBW);
		C50FIR :: setup();
	}
}
