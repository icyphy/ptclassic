defstar {
	name { Floor }
	domain { CGC } 
	desc { Output the greatest integer less than or equal to the input. }
	version { @(#)CGCFloor.pl	1.1 4/20/96 }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { int }
	}
	go {
	        addCode(floor);
	}
	codeblock(floor) {
		double t = (double)$ref(input);
		int it = (int)t;
		// handle rounding towards 0 on some processors
		if (it > t) it -= 1;
		$ref(output) = it;
	}
}
