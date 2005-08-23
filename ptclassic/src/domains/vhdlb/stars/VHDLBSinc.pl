defstar {
	name { Sinc }
	domain {VHDLB}
	desc {
Calculates the sinc function of the input value and outputs it.
Input is in radians.  If input is zero, output is 1.0
	}
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
The sinc function ( sin x / x ) of the input is calculated, using the SIN()
function defined in a VHDL package (IEEE.math_real)

Note that the discrete-time Fourier transform (DTFT) of a sampled sinc
function is an ideal low-pass filter.  Modulating a sampled sinc function by
a cosine function gives an ideal band-pass signal.
        }
	input {
		name { input }
		type { float }
		desc { input angle in radians }
	}
	output {
		name { output }
		type { float }
	}
	go {
	}
}
