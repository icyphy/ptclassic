defcorona {
	name { MemLUT }
	domain { ACS }
	desc {
	    Utilize a local memory as a lookup table
	}
	version {@(#)ACSMemLUT.pl	1.0 1/8/01}
	author { Ken Smith }
	copyright {
	    Copyright (c) 1990-1996 The Regents of the University of California.
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
}
