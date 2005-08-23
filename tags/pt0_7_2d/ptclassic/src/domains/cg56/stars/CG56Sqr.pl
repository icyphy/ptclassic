defstar {
	name { Sqr }
	domain { CG56 }
	desc { Square function.}
	version { @(#)CG56Sqr.pl	1.10 01 Oct 1996 }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	explanation {The output is the square of the input.}
	execTime {
		return 3;
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	constructor {
		noInternalState();
	}
 	codeblock (std) {
	move	$ref(input),x0
   	mpy	x0,x0,a
 	move	a,$ref(output)
	}
 	go {
	 	 addCode(std);
	}
}
