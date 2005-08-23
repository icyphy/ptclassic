defstar {
	name { Multiple }
	domain {VHDLB}
	desc {
Outputs a 1 (logical high) if top input is integer multiple of bottom input,
otherwise the output is zero (logical low).
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
If the top input is an integer multiple of the bottom input (remainder of
the division is zero), then the output is 1.  Otherwise output is zero.  Note
that if the bottom input is zero, than the output is always zero.
	}
	input {
		name { top }
		type { int }
		desc { Is this a multiple of the other input? }
	}
	input {
		name { bottom }
		type { int }
		desc { Reference input (must be non-zero) }
	}
	output {
		name { mult }
		type { int }
		desc { Equals 1 if top is a multiple of bottom }
	}
	go {
	}
}
