defcorona {
	name { IDCT }
	domain { ACS }
	desc { computes the inverse discrete cosine transform of a block of 8 samples }
	version { @(#)ACSIDCT.pl	1.2 09/08/99 }
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
