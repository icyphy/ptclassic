defstar {
	name {FIR}
	domain {SDF}
	desc {
A Finite Impulse Response (FIR) filter.
Coefficients are in the "taps" state variable.
Default coefficients give an 8th order, linear phase lowpass
filter. To read coefficients from a file, replace the default
coefficients with "<fileName".
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF dsp library }
	explanation {
.pp
This star implements a finite-impulse response filter with multirate capability.
.Id "filter, FIR"
.Id "FIR filter"
The default coefficients correspond to an eighth order, equiripple,
linear-phase, lowpass filter.  The 3dB cutoff frequency at about 1/3
of the Nyquist frequency.  To load filter coefficients from a file,
simply replace the default coefficients with the string "<filename".
.pp
It is advisable to use an absolute path name as part of the file name,
especially if you are using the graphical interface.
This will allow the FIR filter to work as expected regardless of
the directory in which the ptolemy process actually runs.
It is best to use tilde's in the filename to reference them to user's
home directory.  This way, future filesystem reorganizations
will have minimal effect.
.pp
When the \fIdecimation\fP (\fIinterpolation\fP)
state is different from unity, the filter behaves exactly
as it were followed (preceded) by a DownSample (UpSample) star.
However, the implementation is much more efficient than
it would be using UpSample and DownSample stars;
a polyphase structure is used internally, avoiding unnecessary use
of memory and unnecessary multiplication by zero.
Arbitrary sample-rate conversions by rational factors can
be accomplished this way.
.pp
To design a filter for a multirate system, simply assume the
sample rate is the product of the interpolation parameter and
the input sample rate, or equivalently, the product of the decimation
parameter and the output sample rate.
.Ir "multirate filter design"
.Ir "filter design, multirate"
.Ir "filter, multirate"
In particular, considerable care must be taken to avoid aliasing.
Specifically, if the input sample rate is f,
then the filter stopband should begin before f/2.
If the interpolation ratio is i, then f/2 is a fraction 1/2i
of the sample rate at which you must design your filter.
.pp
The \fIdecimationPhase\fP parameter is somewhat subtle.
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
.pp
For more information about polyphase filters, see F. J. Harris,
"Multirate FIR Filters for Interpolating and Desampling", in
\fIHandbook of Digital Signal Processing\fR, Academic Press, 1987.
	}
	seealso { ComplexFIR, BiQuad, UpSample, DownSample,
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
	start {
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
	    int phase, tapsIndex;
	    int Interp = interpolation;
	    int Decim = decimation;
	    int outCount = Interp-1;
	    double out, tap;
	
	    // phase keeps track of which phase of the filter coefficients are used.
	    // Starting phase depends on the decimationPhase state.
	    phase = Decim - int(decimationPhase) - 1;   
	
	    // Interpterate once for each input consumed
	    for (int inC = 1; inC <= Decim; inC++) {
		// Produce however many outputs are required
		// for each input consumed
		while (phase < Interp) {
		   out = 0.0;
		   // Compute the inner product.
		   for (int i = 0; i < phaseLength; i++) {
			tapsIndex = i * Interp + phase;
			if (tapsIndex >= taps.size())
			    tap = 0.0;
			else
			    tap = taps[tapsIndex];
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
