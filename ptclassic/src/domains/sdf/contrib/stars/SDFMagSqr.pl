defstar {
	name { MagSqr }
	domain { SDF } 
	desc { 	Magnitude Squared.}
	author { N. Becker }
        copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { SDF user contribution library }
	input {
	  name { input }
	  type { complex }
	}
	output {
		name { output }
		type { float }
	}
	ccinclude { <math.h> }
	go {
	  output%0 << norm(input%0);
	}
}
