defstar {
	name { FloatToCx }
	domain { SDF }
	version { $Id$ }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF conversion palette }
	desc { Converts a float input to a complex type output. }
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
	    output%0 << (Complex)(input%0);
	}
}
