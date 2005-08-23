defstar {
	name { Cosine }
	domain {VHDLB}
	desc { Calculates the cosine function of the input value (in radians) }
	author { Wei-Lun Tsai }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { New_Stars directory }
	htmldoc {
The cosine function of the input is calculated using a separately defined
function in a VHDL package (IEEE.math_real).
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
