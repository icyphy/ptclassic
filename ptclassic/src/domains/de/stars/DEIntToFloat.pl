defstar {
	name { IntToFloat }
	domain { DE }
	version { $Id$ }
	author { Mike J. Chen, John S. Davis, II }
	copyright {
Copyright (c) 1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
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
	    output.put(arrivalTime) << double( input.get() );
	}
}
