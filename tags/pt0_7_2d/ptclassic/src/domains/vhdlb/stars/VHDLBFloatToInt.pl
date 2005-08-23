defstar {
	name { FloatToInt }
	domain {VHDLB}
	desc {
Converts a real number to an integer.  (always round to the nearest integer)
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
Since there is no automatic data type conversions in the VHDL domains of
ptolemy, this star is written to explicitly convert a real input to
an integer output.  Note that the real number is always rounded to
the nearest integer value.
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
