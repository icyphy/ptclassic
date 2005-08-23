defstar {
	name { FloatToInt }
	domain { SDF }
	version { @(#)SDFFloatToInt.pl	1.6 2/12/96 }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	desc { Convert a floating-point input to an integer output. }
	input {
		name { input }
		type { float }
		desc { Input float type }
	}
	output {
		name { output }
		type { int }
		desc { Output int type }
	}
	go {
	    output%0 << int(input%0);
	}
}
