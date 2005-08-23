defstar {
	name { Limit }
	domain {VHDLB}
	desc {
The output of this star is the value of the input limited
to the range between "bottom" and "top" inclusive.

If the specified range is wrong (bottom > top), an error message is generated.
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
The real-valued output is bound by the "top" and "bottom" parameters.  A
ptolemy error is caused if the "top" parameter is set lower than the "bottom"
parameter.
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
		name { top }
		type { float }
		default { 1.0 }
		desc { Upper limit of output signal }
	}
	state {
		name { bottom }
		type { float }
		default { "0.0" }
		desc { Lower limit of output signal }
	}
	setup {
		if( top < bottom )
		  Error::abortRun(*this,"Limiting range incorrectly specified");
	}
	go {
	}
}
