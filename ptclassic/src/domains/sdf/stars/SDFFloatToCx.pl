defstar {
	name { FloatToCx }
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
	desc { Convert a floating-point input to a complex output. }
	input {
		name { input }
		type { float }
		desc { Input float type }
	}
	output {
		name { output }
		type { complex }
		desc { Output complex type }
	}
	go {
	    // We use a temporary variable to avoid gcc2.7.2/2.8 problems
	    Complex t = input%0;
	    output%0 << t;
	}
}
