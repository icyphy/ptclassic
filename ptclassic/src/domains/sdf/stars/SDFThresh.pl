defstar {
	name { Thresh }
	domain { SDF }
	desc {
NOTE:  THIS STAR SHOULD BE REPLACED BY Quantizer.
Compares input values to "threshold" (default 0.5).
Output is 0 if input <= threshold, otherwise it is 1.
	}
	version {$Id$}
	author { D. G. Messerschmitt }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {int}
	}
	defstate {
		name { threshold }
		type { float }
		default {0.5}
		desc { Threshold applied to input. }
	}
	go {
		if( double(input%0) <= double(threshold) )
			output%0 << 0;
		else output%0 << 1;
	}
}
