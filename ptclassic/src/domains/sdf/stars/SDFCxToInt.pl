defstar {
	name { CxToInt }
	domain { SDF }
	version { $Id$ }
	author { Mike J. Chen }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF conversion palette }
	desc { Convert a complex input to an integer output. }
	input {
		name { input }
		type { complex }
		desc { Input complex type }
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
