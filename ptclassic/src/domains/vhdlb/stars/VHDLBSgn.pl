defstar {
	name { Sgn }
	domain {VHDLB}
        desc {
If input is positive (and zero), output is 1; otherwise output is -1.
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
This star calculates the signum function of the input value.  If the input is
greater than or equal to zero, the output is 1.  Otherwise if the input is
negative, the output is -1.
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
