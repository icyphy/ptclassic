defstar {
	name { IntProduct }
	domain { CG56 }
	desc { Two input integer multiplier }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
Input and output  are assumed to be integers (right-justified in word).
}

	input {
		name {firstInput}
		type {FIX}
	}
	input {
		name {secondInput}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	codeblock (multblock) {
	move	$ref(firstInput),x0		; input #1 -> x0
	move	$ref(secondInput),y0         	; input #2 -> y0
	mpy 	x0,y0,a
        asr     a                           ;  needed to handle data as integer
	move 	a0,$ref(output)
	}
 	go {
		gencode(multblock);
  	}
	exectime {
	 	return 4; 
	}
}