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
	htmldoc {
This star implements an FIR filter with
a raised cosine or square-root raised cosine frequency response,
with the excess bandwidth given
by <i>excessBW</i> and the symbol interval (in number of samples)
of the application given by <i>symbol_interval</i>.
The length of the filter (the number of taps) is given by <i>length</i>.
<p>
For the ordinary raised cosine response,
the impulse response of the filter would ideally be
<pre>
       sin(pi n/T)   cos(alpha pi n/T)
h(n) = ----------- * -----------------
         pi n/T      1-(2 alpha n/T)<sup>2</sup>
</pre>
where <i>alpha</i> is <i>excessBW</i> and <i>T</i> is the <i>symbol_interval</i>.
However, this pulse is centered at zero, and we can only implement causal
filters in the SDF domain in Ptolemy.  Hence, the impulse response is
actually
<pre>
g(n) = h(n - M)
</pre>
where <i>M </i>=<i> length/</i>2 if <i>length</i> is even, and <i>M
</i>= (<i>length+</i>1)<i>/</i>2 if <i>length</i> is odd.
The impulse response is simply truncated outside this range, so
the impulse response will generally not be symmetric if <i>length</i> is even
because it will have one more sample to the left than to the right of center.
Unless this extra sample is zero, the filter will not have linear phase
if <i>length</i> is even.
<p>
For some applications, you may wish to apply a window function to
this impulse response.  To do this, feed an impulse into this star,
multiply the output by the output of the <b>Window</b> star, and
store the resulting windowed impulse response in a file. Then, use
the <b>FIR</b> star to implement the filter.
<p>
For the ordinary raised cosine response, the
distance (in number of samples) from the center
to the first zero crossing is given by <i>symbol_interval</i>.
For the square-root raised cosine response, a cascade of two identical
square-root raised cosine filters would be equivalent to a single
ordinary raised cosine filter.
<p>
The impulse response of the square-root raised cosine pulse is given by
<pre>
        4 alpha(cos((1+alpha)pi n/T)+Tsin((1-alpha)pi n/T)/(4n alpha/T))
h(n) = -----------------------------------------------------------------
                     pi sqrt(T)(1-(4 alpha n/T)<sup>2</sup>)
</pre>
This impulse response convolved with itself will, in principle, be equal
to a raised cosine pulse.  However, because of the abrupt rectangular
windowing of the pulse, with low excess bandwidth, this ideal is not
closely approximated except for very long filters.
<p>
The output sample rate is <i>upsample</i> times the input.
This is set by default to 16 because in digital communication systems
this pulse is used for the line coding of symbols, and upsampling is necessary.
Typically, the value of <i>upsample</i> is the same as that of
<i>symbol_interval</i>.
The star is derived from the FIR star.
<a name="raised cosine pulse"></a>
<a name="square-root raised cosine pulse"></a>
<a name="Nyquist pulse"></a>
<h3>References</h3>
<p>[1]  
E. A. Lee and D. G. Messerchmitt,
<i>Digital Communication,</i> Kluwer Academic Publishers, Boston, 1988.
<p>[2]  
I. Korn, <i>Digital Communications</i>, Van Nostrand Reinhold, New York, 1985.
	}
	seealso {FIR, Window, pulses}
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
		taps.resize (length);
		int center = int(length)/2;
		for (int i = 0; i < int(length); i++) {
		    if (int(square_root))
			taps[i] = Ptdsp_SqrtRaisedCosine(i - center,
					int(symbol_interval), int(excessBW));
		    else
			taps[i] = Ptdsp_RaisedCosine(i - center,
					int(symbol_interval), int(excessBW));
		}
		SDFFIR :: setup();
	}
}
