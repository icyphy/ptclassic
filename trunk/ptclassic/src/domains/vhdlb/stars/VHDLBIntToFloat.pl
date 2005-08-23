defstar {
	name { IntToFloat }
	domain {VHDLB}
	desc { Converts an integer to a real number. }
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
ptolemy, this star is written to explicitly convert an integer input to a
real-valued output.
	}
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { float }
	}
	go {
	}
}
