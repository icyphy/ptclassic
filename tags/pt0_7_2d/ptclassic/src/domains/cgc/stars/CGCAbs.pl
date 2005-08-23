defstar {
	name { Abs }
	domain { CGC } 
	desc { Output the absolute value of the input value. }
	version { @(#)CGCAbs.pl	1.2 01 Oct 1996 }
	author { William Chen }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	htmldoc {
Outputs absolute value of the input.
<a name="absolute value"></a>
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
