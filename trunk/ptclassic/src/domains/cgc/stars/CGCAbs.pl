defstar {
	name { Abs }
	domain { CGC } 
	desc { Output the absolute value of the input value. }
	version { $Id$ }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
Outputs absolute value of the input.
.Id "absolute value"
	}
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	go {
	        addCode(abs);
	}
	codeblock(abs) {
		double t = $ref(input);
		if (t < 0.0) t = -t;
		$ref(output) = t;
	}
}
