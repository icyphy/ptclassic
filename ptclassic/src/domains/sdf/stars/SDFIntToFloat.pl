defstar {
	name { IntToFloat }
	domain { SDF }
	version { $Id$ }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	desc { Convert an integer input to a floating-point output. }
	input {
		name { input }
		type { int }
		desc { Input integer type }
	}
	output {
		name { output }
		type { float }
		desc { Output float type }
	}
	go {
	    output%0 << double(input%0);
	}
}
