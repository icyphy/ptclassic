defcorona {
	name { NL }
	domain { ACS }
	desc { Non-linear operation including a low threshold, gain adjustment for linear region, and a upper limit. }
	version { @(#)ACSNL.pl	1.2 09/08/99 }
	author { James Lundblad }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
	location { ACS main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
}
