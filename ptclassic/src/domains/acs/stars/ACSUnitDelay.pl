defcorona {
    name { UnitDelay }
    domain { ACS }
    desc {
	Generates a pipelined delay element for multiple lines
	}
    version {@(#)ACSUnitDelay.pl	1.4 05/15/00}
    author { K. Smith }
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
