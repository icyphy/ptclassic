defstar {
	name { Add }
	domain { C50 }
	desc { Add any number of inputs, producing an output. }
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
The inputs are added and the result is written on the output.
	}
	inmulti {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	codeblock (std) {
	mar	*,AR5
	lar	AR5,#$addr(input#2)	; address 2nd input -> AR5
	lar	AR6,#$addr(input#1)	; address 1st input -> AR6
	lar	AR7,#$addr(output)
	lacc	*,15,AR5		; 2nd input -> ACC
	}
	codeblock (loop, "int i") {
	lar	AR5,#$addr(input#@i)
	add	*,15
	}
	codeblock (nosat) {
	mar	*,AR6
	add	*,15,AR7		; add 1st input
	sach	*,1			; result -> output
	}
	codeblock (one) {
	mar 	*,AR5
	lar	AR5,#$addr(input#1)	; just move data from in to out
	bldd	*,#$addr(output)
	}
	constructor {
		noInternalState();
	}
	go {
		if (input.numberPorts() == 1) {
			addCode(one);
		}
		else {
			addCode(std);
			for (int i = 3; i <= input.numberPorts(); i++) {
				addCode(loop(i));
			}
			// FIXME: No support for saturation arithmetic
			// See the CG56 Add star
	 		addCode(nosat);
		}
	}
	exectime {
		if (input.numberPorts() == 1) return 4;
		else return input.numberPorts() + 6;
	}
}
