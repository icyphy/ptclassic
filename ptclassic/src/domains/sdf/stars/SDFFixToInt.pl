defstar {
	name { FixToInt }
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
	desc { Convert a fixed-point input to an integer output. }
	input {
		name { input }
		type { fix }
		desc { Input fix type }
	}
	output {
		name { output }
		type { int }
		desc { Output int type }
	}
	go {
	    output%0 << (int)(input%0);
	}
}
