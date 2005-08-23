defstar {
	name { Log }
	domain {VHDLB}
	desc { Calculates natural logarithm of the input. }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
The natural logarithm of the input value is calculated, using a separately
defined function in a VHDL package (IEEE.math_real)
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
		desc { The default minimum value to use if the input is not
                       valid (less than or equal to zero)
		}
	}
	go {
	}
}
