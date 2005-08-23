defstar {
	name { Floor }
	domain { SDF } 
	desc { Output the greatest integer less than or equal to the input. }
	version {@(#)SDFFloor.pl	1.9 11/15/95}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { int }
	}
	go {
		double t = double(input%0);
		int it = int(t);
		// handle rounding towards 0 on some processors
		if (it > t) it -= 1;
		output%0 << it;
	}
}
