defstar {
	name { Mpy }
	domain { CG56 }
	desc { Multipy any number of inputs, producing an output. }
	version { $Id$ }
	author { Chih-Tsung Huang, ported from Gabriel }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 demo library }
	explanation {
.Id "multiplication"
The inputs are multiplied and the result is written on the output.
	}
	inmulti {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
        state  {
                name { inputNum }
                type { int }
                default { 0 }
                desc { input#() }
                attributes { A_NONCONSTANT|A_NONSETTABLE }
        }
	codeblock (std) {
	move	$ref(input#1),x0	 ; 1st input -> x0
        move	$ref(input#2),y0         ; 2nd input -> y0
	}
	codeblock (sat) {
	mpy	x0,y0,a
	move	a,$ref(output)	
	}
	codeblock (one) {
	move	$ref(input#1),x0	; just move data from in to out
	move	x0,$ref(output)
	}
        codeblock(loop) {
        mpy     x0,y0,a     $ref(input#inputNum),x0
	move    a,y0
        }

	go {
		if (input.numberPorts() == 1) {
			addCode(one);
			return;
		}
		addCode(std);
		for (int i = 3; i <= input.numberPorts(); i++) {
	                inputNum=i;
			addCode(loop);
		}
	        addCode(sat);
	}
	exectime {
		if (input.numberPorts() == 1) return 2;
		else return input.numberPorts() + 2;
	}
}
