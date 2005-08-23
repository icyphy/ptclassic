defstar {
	name {FIR}
	domain {SDF}
	desc {
A finite impulse response (FIR) filter.  Coefficients are specified by
the "taps" parameter.  The default coefficients give an 8th-order, linear
phase lowpass filter. To read coefficients from a file, replace the default
coefficients with "&lt; fileName", preferably specifying a complete path.
Rational sampling rate changes, implemented by polyphase multirate filters,
is also supported.
	}
	version {@(#)SDFFIR.pl	1.18 10/06/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF dsp library }
	htmldoc {
<p>
This star implements a finite-impulse response filter with multirate capability.
<a name="filter, FIR"></a>
<a name="FIR filter"></a>
The default coefficients correspond to an eighth order, equiripple,
linear-phase, lowpass filter.  The 3dB cutoff frequency at about 1/3
of the Nyquist frequency.  To load filter coefficients from a file,
simply replace the default coefficients with the string "&lt;filename".
<p>
It is advisable to use an absolute path name as part of the file name,
especially if you are using the graphical interface.
This will allow the FIR filter to work as expected regardless of
the directory in which the Ptolemy process actually runs.
It is best to use tilde's in the filename to reference them to user's
home directory.  This way, future filesystem reorganizations
will have minimal effect.
<p>
When the <i>decimation</i> (<i>interpolation</i>)
state is different from unity, the filter behaves exactly
as it were followed (preceded) by a DownSample (UpSample) star.
However, the implementation is much more efficient than
it would be using UpSample and DownSample stars;
a polyphase structure is used internally, avoiding unnecessary use
of memory and unnecessary multiplication by zero.
Arbitrary sample-rate conversions by rational factors can
be accomplished this way.
<p>
To design a filter for a multirate system, simply assume the
sample rate is the product of the interpolation parameter and
the input sample rate, or equivalently, the product of the decimation
parameter and the output sample rate.
<a name="multirate filter design"></a>
<a name="filter design, multirate"></a>
<a name="filter, multirate"></a>
In particular, considerable care must be taken to avoid aliasing.
Specifically, if the input sample rate is f,
then the filter stopband should begin before f/2.
If the interpolation ratio is i, then f/2 is a fraction 1/2i
of the sample rate at which you must design your filter.
<p>
The <i>decimationPhase</i> parameter is somewhat subtle.
It is exactly equivalent the phase parameter of the DownSample star.
Its interpretation is as follows; when decimating,
samples are conceptually discarded (although a polyphase structure
does not actually compute the discarded samples).
If you are decimating by a factor of three, then you will select
one of every three outputs, with three possible phases.
When decimationPhase is zero (the default),
the latest (most recent) samples are the ones selected.
The decimationPhase must be strictly less than
the decimation ratio.
<p>
For more information about polyphase filters, see F. J. Harris,
"Multirate FIR Filters for Interpolating and Desampling", in
<i>Handbook of Digital Signal Processing</i>, Academic Press, 1987.
	}
	seealso { FIRCx, Biquad, UpSample, DownSample,
		  firDemo, interp, multirate }
	input {
		name {signalIn}
		type {float}
	}
	output {
		name {signalOut}
		type {float}
	}
	defstate {
		name {taps}
		type {floatarray}
		default {
	"-.040609 -.001628 .17853 .37665 .37665 .17853 -.001628 -.040609"
		}
		desc { Filter tap values. }
	}
	defstate {
		name {decimation}
		type {int}
		default {1}
		desc {Decimation ratio.}
	}
	defstate {
		name {decimationPhase}
		type {int}
		default {0}
		desc {Downsampler phase.}
	}
	defstate {
		name {interpolation}
		type {int}
		default {1}
		desc {Interpolation ratio.}
	}
	protected {
		int phaseLength;
	}
	setup {
		int d = decimation;
		int i = interpolation;
		int dP = decimationPhase;
		signalIn.setSDFParams(d, d+1+(taps.size()/i));
		signalOut.setSDFParams(i, i-1);
		if (dP >= d) {
			Error::abortRun (*this, ": decimationPhase too large");
			return;
		}
		// The phaseLength is ceiling(taps.size/interpolation)
		// It is a protected instance variable.
		phaseLength = taps.size() / i;
		if ((taps.size() % i) != 0) phaseLength++;
	}
	go {
	    int Interp = interpolation;
	    int Decim = decimation;
	    int outCount = Interp - 1;
	
	    // phase keeps track of which phase of the filter coefficients
	    // are used. Starting phase depends on the decimationPhase state.
	    int phase = Decim - int(decimationPhase) - 1;   
	
	    // Interpterate once for each input consumed
	    for (int inC = 1; inC <= Decim; inC++) {
		// Produce however many outputs are required
		// for each input consumed
		while (phase < Interp) {
		   double out = 0.0;
		   // Compute the inner product.
		   for (int i = 0; i < phaseLength; i++) {
			int tapsIndex = i * Interp + phase;
			double tap = 0.0;
			if (tapsIndex < taps.size()) tap = taps[tapsIndex];
			out += tap * double(signalIn%(Decim - inC + i));
		   }
		   // note: output%0 is the last output chronologically
		   signalOut%(outCount--) << out;
		   phase += Decim;
		}
		phase -= Interp;
	    }
	}
}
