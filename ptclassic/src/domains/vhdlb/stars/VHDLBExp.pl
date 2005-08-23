defstar {
	name { Exp }
	domain {VHDLB}
	desc { Calculates the exponential function of the input value }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
The exponential function of the input value is calculated, using a separately
defined function in a VHDL package (IEEE.math_real).
        }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	go {
	}
}
