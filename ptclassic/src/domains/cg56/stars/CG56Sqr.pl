defstar {
	name { Sqr }
	domain { CG56 }
	desc { Square function.}
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 nonlinear functions library }
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
 	codeblock (std) {
	move	$ref(input),x0
   	mpy	x0,x0,a
 	move	a,$ref(output)
	}
 	go {
	 	 addCode(std);
	}
}
