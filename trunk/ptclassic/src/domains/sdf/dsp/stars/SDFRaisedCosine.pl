defstar {
	name { RaisedCosine }
	domain { SDF }
	derivedFrom { FIR }
	desc {
An FIR filter with a magnitude frequency response that is shaped
like the standard raised cosine or square-root raised cosine
used in digital communications.  By default, the star upsamples
by a factor of 16, so 16 outputs will be produced for each input
unless the "interpolation" parameter is changed.
	}
	version { $Id$ }
	author { J. T. Buck and E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	explanation {
This star implements an FIR filter with
a raised cosine or square-root raised cosine frequency response,
with the excess bandwidth given
by \fIexcessBW\fR and the symbol interval (in number of samples)
of the application given by \fIsymbol_interval\fP.
The length of the filter (the number of taps) is given by \fIlength\fP.
.pp
For the ordinary raised cosine response,
the impulse response of the filter would ideally be
.EQ
h(n) ~=~ left ( { sin ( pi n / T ) } over { pi n / T } right )
left ( { cos ( alpha pi n / T ) } over { 1 ~-~ (2 alpha n / T) sup 2 } right )
.EN
where $alpha$ is \fIexcessBW\fR and $T$ is the \fIsymbol_interval\fR.
However, this pulse is centered at zero, and we can only implement causal
filters in the SDF domain in Ptolemy.  Hence, the impulse response is
actually
.EQ
g(n) ~=~ h(n - M)
.EN
where $M ~=~ length/2$ if $length$ is even, and $M ~=~ (length+1)/2$ if
$length$ is odd.
The impulse response is simply truncated outside this range, so
the impulse response will generally not be symmetric if $length$ is even
because it will have one more sample to the left than to the right of center.
Unless this extra sample is zero, the filter will not have linear phase
if $length$ is even.
.pp
For some applications, you may wish to apply a window function to
this impulse response.  To do this, feed an impulse into this star,
multiply the output by the output of the \fBWindow\fR star, and
store the resulting windowed impulse response in a file. Then, use
the \fBFIR\fR star to implement the filter.
.pp
For the ordinary raised cosine response, the
distance (in number of samples) from the center
to the first zero crossing is given by \fIsymbol_interval\fP.
For the square-root raised cosine response, a cascade of two identical
square-root raised cosine filters would be equivalent to a single
ordinary raised cosine filter.
.pp
The impulse response of the square-root raised cosine pulse is given by
.EQ
h(n) ~=~ {4 alpha} over {pi sqrt T} cdot
{ cos ( (1 ~+~ alpha ) pi n / T ) ~+~
{ T sin ((1 ~-~ alpha ) pi n/T) } over {4 alpha n/T}} over
{1 ~-~ (4 alpha n/T) sup 2 } ~.
.EN
This impulse response convolved with itself will, in principle, be equal
to a raised cosine pulse.  However, because of the abrupt rectangular
windowing of the pulse, with low excess bandwidth, this ideal is not
closely approximated except for very long filters.
.pp
The output sample rate is \fIupsample\fP times the input.
This is set by default to 16 because in digital communication systems
this pulse is used for the line coding of symbols, and upsampling is necessary.
Typically, the value of \fIupsample\fP is the same as that of
\fIsymbol_interval\fR.
The star is derived from the FIR star.
.Id "raised cosine pulse"
.Id "square-root raised cosine pulse"
.Id "Nyquist pulse"
.UH REFERENCES
.ip [1]
E. A. Lee and D. G. Messerchmitt,
\fIDigital Communication,\fR Kluwer Academic Publishers, Boston, 1988.
.ip [2]
I. Korn, \fIDigital Communications\fR, Van Nostrand Reinhold, New York, 1985.
	}
	seealso {FIR, Window, pulses}
	ccinclude { "PTDSPRaisedCosine.h" }
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
		taps.resize (length);
		int center = int(length)/2;
		for (int i = 0; i < int(length); i++) {
		    if (int(square_root))
			taps[i] = PTDSPSqrtRaisedCosine(i - center,
					int(symbol_interval), int(excessBW));
		    else
			taps[i] = PTDSPRaisedCosine(i - center,
					int(symbol_interval), int(excessBW));
		}
		SDFFIR :: setup();
	}
}
