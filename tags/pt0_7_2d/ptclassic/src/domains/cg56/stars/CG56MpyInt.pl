defstar {
	name { MpyInt }
	domain { CG56 }
	desc { Multiply any number of inputs, producing an output. }
	version { @(#)CG56MpyInt.pl	1.16	01 Oct 1996 }
	author { Chih-Tsung Huang, ported from Gabriel }
	acknowledge { Martha Fratt, who wrote the Gabriel version }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="multiplication"></a>
The inputs are multiplied and the result is written on the output.
This star is similar to the Mpy star.
	}
	inmulti {
		name {input}
		type {int}
	}
	output {
		name {output}
		type {int}
	}
	constructor {
		noInternalState();
	}

	codeblock (copyInput) {
	move	$ref(input#1),x0		; just copy input to output
	move	x0,$ref(output)
	}

	codeblock (multiplyStart) {
	move	$ref(input#1),x0			; 1st input -> x0
        move	$ref(input#2),y0			; 2nd input -> y0
	}

        codeblock(multiply,"int i") {
        mpy     x0,y0,a     $ref(input#@i),x0	; a = x0 * y0, get next input
	asr	a			; adjust for integer multiplication
	move    a0,y0			; y0 = integer result in accumulator
        }

	codeblock (multiplyEnd) {
	mpy	x0,y0,a			; a = x0 * y0
	asr	a			; adjust for integer multiplication
	move	a0,$ref(output)			; return the integer result
	}

	go {
		if (input.numberPorts() == 1) {
			addCode(copyInput);
			return;
		}

		addCode(multiplyStart);
		for (int i = 3; i <= input.numberPorts(); i++) {
			addCode(multiply(i));
		}
	        addCode(multiplyEnd);
	}
	exectime {
		if (input.numberPorts() == 1) return 2;
		return 2 + input.numberPorts();
	}
}
