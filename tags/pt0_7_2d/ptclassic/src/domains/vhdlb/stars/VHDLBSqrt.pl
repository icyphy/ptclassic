defstar {
	name { Sqrt }
	domain {VHDLB}
	desc { Calculates the square root of the input number. }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
Uses a predefined SQRT() function in a VHDL package (IEEE.math_real) to
calculate the square root of the input number.
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
