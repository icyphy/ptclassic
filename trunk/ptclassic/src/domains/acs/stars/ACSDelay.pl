defcorona {
	name { Delay }
	domain { ACS }
	desc {
	    Generates a pipelined delay element for multiple lines
	}
	version { $Id$}
	author { K. Smith }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	input {
		name {input}
		type {FLOAT}
	}
	output {
		name {output}
		type {FLOAT}
	    }
	defstate {
		name { delays }
		type { int }
		default { 1 }
		desc { Pipeline delays. }
	}
}  