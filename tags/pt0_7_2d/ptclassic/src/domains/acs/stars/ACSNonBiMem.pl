defcorona {
	name { NonBiMem }
	domain { ACS }
	desc {
	    Converts a bidirectional memory interface into individual
	    source/sink memory lines
	}
	version {@(#)ACSNonBiMem.pl	1.3 09/08/99}
	author { K. Smith }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { ACS main library }
	input {
		name {bidir_input}
		type {FLOAT}
	}
	output {
		name {in_mem}
		type {FLOAT}
	    }
	output {
		name {out_mem}
		type {FLOAT}
	    }
}  
