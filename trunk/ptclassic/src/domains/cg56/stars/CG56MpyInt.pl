defstar {
	name { MpyInt }
	domain { CG56 }
	desc { Two input integer multiplier.  The output wraps around on overflow.}
	version { $Id$ }
	acknowledge { Gabriel version by Martha Fratt }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 arithmetic library }
	explanation {
.Id "multiplication, integer"
.Id "integer multiplication"
Input and output are assumed to be integers (right-justified in word).
}

	input {
		name {firstInput}
		type {int}
	}
	input {
		name {secondInput}
		type {int}
	}
	output {
		name {output}
		type {int}
	}
	codeblock (multblock) {
	move	$ref(firstInput),x0		; input #1 -> x0
	move	$ref(secondInput),y0         	; input #2 -> y0
	mpy 	x0,y0,a
        asr     a                           ;  needed to handle data as integer
	move 	a0,$ref(output)
	}
 	go {
		addCode(multblock);
  	}
	exectime {
	 	return 4; 
	}
}
