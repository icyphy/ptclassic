defcorona {
	name { FIR }
	domain { ACS }
	desc { 
A finite impulse response (FIR) filter.  Coefficients are specified by
the "taps" parameter.  The default coefficients give an 8th-order, linear
phase lowpass filter. To read coefficients from a file, replace the default
coefficients with "&lt; fileName", preferably specifying a complete path.
Rational sampling rate changes, implemented by polyphase multirate filters,
is also supported.
	}
	version { @(#)ACSFIR.pl	1.2 09/08/99 }
	author { James Lundblad }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
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
}
