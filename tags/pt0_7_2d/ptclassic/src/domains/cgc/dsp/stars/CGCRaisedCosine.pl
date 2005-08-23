defstar {
	name { RaisedCosine }
	domain { CGC }
	derivedFrom { FIR }
	desc {
An FIR filter with a magnitude frequency response that is shaped
like the standard raised cosine or square-root raised cosine
used in digital communications.
	}
	version { @(#)CGCRaisedCosine.pl	1.4	10/16/97 }
	author { Joseph T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	htmldoc {
See the SDFRaisedCos star.
	}
	seealso { FIR, Window }
	ccinclude { "ptdspRaisedCosine.h" }
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
		interpolation.setInitValue("16");
	}
	setup {
		if (double(excessBW) < 0.0)
		    Error::abortRun(*this, "Invalid excess bandwidth");
		if (int(symbol_interval) <= 0)
		    Error::abortRun(*this, "Invalid symbol interval");
		taps.resize(length);
		int center = int(length)/2;
		for (int i = 0; i < int(length); i++) {
		    if (int(square_root))
			taps[i] = Ptdsp_SqrtRaisedCosine(i - center,
					int(symbol_interval), excessBW);
		    else
			taps[i] = Ptdsp_RaisedCosine(i - center,
					int(symbol_interval), excessBW);
		}
		CGCFIR :: setup();
	}
}
