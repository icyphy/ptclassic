defstar {
	name { RaisedCosCx }
	domain { SDF }
	derivedFrom { FIRCx }
	desc {
Output a complex raised-cosine Nyquist pulse.
	}
	version { $Id$ }
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF dsp library }
	explanation {
This star outputs a complex raised cosine pulse with excess bandwidth given
by \fIexcessbw\fR and pulse width (distance from center to first
zero crossing) given by \fIP\fP.  The length of the filter (number
of taps) is \fIN\fP, and the output sample rate is \fIupsample\fP
times the input.  This star is implemented by deriving from the ComplexFIR
star.
.Id "raised cosine pulse, complex"
.Id "Nyquist pulse, complex"
.pp
See "Digital Communication" by Lee and Messerschmitt for a discussion
of raised cosine pulses in communications systems.
	}
	seealso {FIRCx, RaisedCos}
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
	protected {
		char ivbuf[20];
	}	
	constructor {
		// taps are no longer constant or settable
		taps.clearAttributes(A_CONSTANT|A_SETTABLE);
		// fix interpolation default
		interpolation.setValue("16");
		// make decimation parameters invisible
		decimation.clearAttributes(A_SETTABLE);
		decimationPhase.clearAttributes(A_SETTABLE);
	}
	setup {
		taps.resize(int(N));
		int center = int(N)/2;
		for (int i = 0; i < int(N); i++)
			taps[i] = rcos (i - center, P, excessBW);
		SDFFIRCx :: setup();
	}
}

