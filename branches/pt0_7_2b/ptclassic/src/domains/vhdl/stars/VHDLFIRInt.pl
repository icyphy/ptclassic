defstar {
	name {FIRInt}
	domain {VHDL}
	desc {
A Finite Impulse Response (FIR) filter.
Coefficients are in the "taps" state variable.
Default coefficients give an 8th order, linear phase lowpass
filter. To read coefficients from a file, replace the default
coefficients with "<fileName".
	}
	version { $Id$ }
	author { Soonhoi Ha, Mike Williamson }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
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
		type {int}
	}
	output {
		name {signalOut}
		type {int}
	}
	defstate {
		name {taps}
		type {intarray}
		default {
	"9 8 3 5 5 3 8 9"
		}
		desc { Filter tap values. }
	}
	go {
	  StringList out;
	  int tapSize = taps.size();
	  // Perform calculation.
	  out << "$ref(signalOut) $assign(signalOut) ";
	  out << "$ref(signalIn,0)*$ref(taps,0)";
	  for (int i=1; i<int(tapSize); i++) {
	    out << " + $ref(signalIn,";
	    out << i;
	    out << ")*$ref(taps,";
	    out << i;
	    out << ")";
	  }
	  out << ";\n";
	  addCode(out);
	  out.initialize();
	}
}