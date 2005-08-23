defstar {
	name { Floor }
	domain {VHDLB}
	desc { Output the greatest integer less than or equal to the input. }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
Uses the FLOOR() function defined in the VHDL package IEEE.math_real to
calculate the biggest integer smaller than or equal to the input number (a
real-valued number)
        }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { int }
	}
	go {
	}
}
