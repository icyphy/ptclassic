ident {
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

Programmer:  D. G. Messerschmitt and E. A. Lee
Date of creation: 1/16/90
Modifications: 8/4/90: added multirate capability and documentation: EAL

ComplexFIR implements a finite-impulse response filter with multirate
capability.  The default coefficients correspond to a real eighth order,
equiripple, linear-phase, lowpass filter.
The 3dB cutoff frequency at about 1/3
of the Nyquist frequency.  To load filter coefficients from a file,
simply replace the default coefficients with the string "<filename".
The real and imaginary parts should be enclosed in parenthesis,
as follows: (0.1,0.3).

It is advisable to use an absolute path name as part of the file name,
especially if you are using the graphical interface.
This will allow the filter to work as expected regardless of
the directory in which the ptolemy process actually runs.
It is best to use tilde's in the filename.

When the \fIdecimation\fP (\fIinterpolation\fP)
state is different from unity, the filter behaves exactly
as it were followed (preceded) by a DownSample (UpSample) star.
However, the implementation is much more efficient than
it would be using UpSample and DownSample stars;
a polyphase structure is used internally, avoiding unnecessary use
of memory and unnecessary multiplication by zero.
Arbitrary sample-rate conversions by rational factors can
be accomplished this way.

####### NOTE THAT IF THESE STATES ARE CHANGED, THE SCHEDULER MUST
####### BE RE-RUN.  THERE IS NO MECHANISM NOW FOR ENFORCING THIS.

To design a filter for a multirate system, simply assume the
sample rate is the product of the interpolation parameter and
the input sample rate, or equivalently, the product of the decimation
parameter and the output sample rate.
In particular, considerable care must be taken to avoid aliasing.
Specifically, if the input sample rate is f,
then the filter stopband should begin before f/2.
If the interpolation ratio is i, then f/2 is a fraction 1/2i
of the sample rate at which you must design your filter.

####### POINT TO DEMO EXAMPLES

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

For more information about polyphase filters, see F. J. Harris,
"Multirate FIR Filters for Interpolating and Desampling", in
\fIHandbook of Digital Signal Processing\fR, Academic Press, 1987.

SEE ALSO

FIR(X), BiQuad(X), UpSample(X), DownSample(X)

**************************************************************************/
}

defstar {
	name {ComplexFIR}
	domain {SDF}
	desc {
	   "A complex Finite Impulse Response (FIR) filter.\n"
	   "Coefficients are in the 'taps' state variable.\n"
	   "Default coefficients give an 8th order, linear phase lowpass\n"
	   "filter. To read coefficients from a file, replace the default\n"
	   "coefficients with \"<fileName\".  Real and imaginary parts\n"
	   "should be paired with parentheses, e.g. (1.0, 0.0).\n"
	}

	input {
		name {signalIn}
		type {complex}
	}
	output {
		name {signalOut}
		type {complex}
	}
	defstate {
		name {taps}
		type {complexarray}
		default {
		"(-.040609,0.0) (-.001628,0.0) (.17853,0.0) (.37665,0.0)
		    (.37665,0.0) (.17853,0.0) (-.001628,0.0) (-.040609,0.0)"
		}
		desc { "filter tap values" }
	}
	defstate {
		name {decimation}
		type {int}
		default {1}
		desc {"decimation ratio"}
	}
	defstate {
		name {decimationPhase}
		type {int}
		default {0}
		desc {"downsampler phase"}
	}
	defstate {
		name {interpolation}
		type {int}
		default {1}
		desc {"interpolation ratio"}
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
	    int outCount = int(interpolation)-1;
	    Complex out, tap;
	
	    // phase keeps track of which phase of the filter coefficients are used.
	    // Starting phase depends on the decimationPhase state.
	    phase = int(decimation) - int(decimationPhase) - 1;   
	
	    // Iterate once for each input consumed
	    for (int inC = 1; inC <= int(decimation); inC++) {
		// Produce however many outputs are required for each input consumed
		while (phase < int(interpolation)) {
		   out = 0.0;
		   // Compute the inner product.
		   for (int i = 0; i < phaseLength; i++) {
			tapsIndex = i * int(interpolation) + phase;
			if (tapsIndex >= taps.size())
			    tap = 0.0;
			else
			    tap = taps[tapsIndex];
			out += tap * Complex(signalIn%(int(decimation) - inC + i));
		   }
		   // note: output%0 is the last output chronologically
		   signalOut%(outCount--) << out;
		   phase += int(decimation);
		}
		phase -= int(interpolation);
	    }
	}
}