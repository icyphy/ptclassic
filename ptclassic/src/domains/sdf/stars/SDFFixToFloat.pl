defstar {
	name { FixToFloat }
	domain { SDF }
	version { @(#)SDFFixToFloat.pl	1.6 2/12/96 }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	desc { Convert a fixed-point input to a floating-point output. }
	input {
		name { input }
		type { fix }
		desc { Input fix type }
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
