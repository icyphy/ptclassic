defstar {
	name { DB }
	domain {VHDLB}
	desc {
Convert input to a decibels (dB) scale. Zero and negative values
are assigned the value "min" (default -100). The "inputIsPower"
parameter should be set to "Yes"  if the input signal is a power
measurement (vs. an amplitude measurement).
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
Converts the input value into a decibel number.  This star is useful for cer-
tain signal processing systems.  Note that the conversion formula is
20 * log( input ) unless the input is a power measurement, in which case the
formula used is 10 * log( input ).

For inputs less than or equal to zero, the output is defaulted to the "min"
value.  For inputs greater than zero, the output is either the "min" or the
value obtained by the formula gain * log( input ), whichever is greater.
        }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	state {
		name { min }
		type { float }
		default {"-100"}
		desc { Minimum output value. }
	}
	state {
		name { inputIsPower }
		type { int }
		default {"FALSE"}
		desc { TRUE if input is power measurement, else FALSE if it's
		       amplitude measurement }
	}
	go {
	}
}
