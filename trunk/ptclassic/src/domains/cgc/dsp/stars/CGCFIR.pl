defstar {
	name {FIR}
	domain {CGC}
	desc {
A Finite Impulse Response (FIR) filter.
Coefficients are in the "taps" state variable.
Default coefficients give an 8th order, linear phase lowpass
filter. To read coefficients from a file, replace the default
coefficients with "<fileName".
	}
	version {$Id$}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
.pp
This star implements a finite-impulse response filter with multirate capability.
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
	defstate {
		name {phaseLength}
		type {int}
		default {0}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	defstate {
		name {tapSize}
		type {int}
		default {0}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
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
		int temp = taps.size() / i;
		tapSize = taps.size();
		if ((taps.size() % i) != 0)  temp++;
		phaseLength = temp;
	}

   codeblock(bodyDecl) {
	int phase, tapsIndex, inC, i;
	int outCount = $val(interpolation) - 1;
	int inPos;
	double out, tap;
   }
   codeblock(body) {
	/* phase keeps track of which phase of the filter coefficients is used.
	   Starting phase depends on the decimationPhase state. */
	phase = $val(decimation) - $val(decimationPhase) - 1;   
	
	/* Iterate once for each input consumed */
	for (inC = 1; inC <= $val(decimation) ; inC++) {

		/* Produce however many outputs are required for each 
		   input consumed */
		while (phase < $val(interpolation)) {
			out = 0.0;

			/* Compute the inner product. */
			for (i = 0; i < $val(phaseLength); i++) {
				tapsIndex = i * $val(interpolation) + phase;
				if (tapsIndex >= $val(tapSize))
			    		tap = 0.0;
				else
			 		tap = $ref2(taps,tapsIndex);
				inPos = $val(decimation) - inC + i;
				out += tap * $ref2(signalIn,inPos);
			}
			$ref2(signalOut,outCount) = out;
			outCount--;;
			phase += $val(decimation);
		}
		phase -= $val(interpolation);
	}
   }
	go {
		addCode(bodyDecl);
		addCode(body);
	}
}
