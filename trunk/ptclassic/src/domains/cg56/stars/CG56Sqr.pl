defstar {
	name { Sqr }
	domain { CG56 }
	desc {
The Output is Squared.
}
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
The input is squared.
	}
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
