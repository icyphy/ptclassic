defstar {
	name { FloatToInt }
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
	    output.put(arrivalTime) << int( input.get() );
	}
}
