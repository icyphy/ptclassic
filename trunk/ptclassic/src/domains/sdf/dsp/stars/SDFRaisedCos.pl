defstar {
	name { RaisedCos }
	domain { SDF }
	derivedFrom { FIR }
	desc {
An FIR filter with a magnitude frequency response shaped
like the standard raised cosine used in digital communications.
By default, the star upsamples by a factor of 16, so 16 outputs
will be produced for each input unless the "interpolation"
parameter is changed.
	}
	version { $Id$ }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
This star implements an FIR filter with
a raised cosine frequency response, with the excess bandwidth given
by \fIexcessbw\fR and the distance from center to first
zero crossing given by \fIP\fP.
The length of the filter (the number of taps) is \fIN\fP.
Ideally, the impulse response of the filter would be
.EQ
h(n) ~=~ left ( { sin ( pi n / P ) } over { pi n / P } right )
left ( { cos ( alpha pi n / P ) } over { 1 minus 2 alpha n / P } right )
.EN
where $alpha$ is \fIexcessbw\fR.
However, this pulse is centered at zero, and we can only implement causal
filters in the SDF domain in Ptolemy.  Hence, the inpulse response is
actually
.EQ
g(n) ~=~ h(n - M)
.EN
where $M ~=~ N/2$ if $N$ is even, and $M ~=~ (N+1)/2$ if $N$ is odd.
Since the impulse response is simply truncated outside this range,
note that if $N$ is even the impulse response will not be symmetric.
It will have one more sample to the left than to the right of center.
Unless this extra sample is zero, the filter will not have linear phase
if $N$ is even.
.sp
The output sample rate is \fIupsample\fP times the input.
This is set by default to 16 becuase in digital communication systems
this pulse is used for line coding of symbols, and upsampling is necessary.
The star is implemented by deriving from the FIR star.
.Id "raised cosine pulse"
.Id "Nyquist pulse"
.UH REFERENCES
.ip [1]
E. A. Lee and D. G. Messerchmitt,
"Digital Communication," Kluwer Academic Publishers, 1988.
	}
	seealso {FIR}
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
		interpolation.setInitValue("16");
		// make decimation parameters invisible
		decimation.clearAttributes(A_SETTABLE);
		decimationPhase.clearAttributes(A_SETTABLE);
	}
	setup {
		taps.resize (N);
		int center = int(N)/2;
		for (int i = 0; i < int(N); i++)
			taps[i] = rcos (i - center, P, excessBW);
		SDFFIR :: setup();
	}
}

