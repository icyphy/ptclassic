defstar {
	name { AddInt }
	domain { CG56 }
	desc { Add any number of integer inputs, producing an integer output. }
	version { $Id$ }
	author { Joseph T. Buck and Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	explanation {
The inputs are added and the result is written on the output.
	}
	inmulti {
		name {input}
		type {int}
	}
	output {
		name {output}
		type {int}
	}
	state {
		name { saturation }
		type { int }
		default { "YES" }
		desc { If true, use saturation arithmetic }
	}
	constructor {
		noInternalState();
	}

	// We use a1 instead a because we are working with integers

	codeblock(copyInput) {
	move	$ref(input#1),x0	; just move data from in to out
	move	x0,$ref(output)
	}

	codeblock(addStart) {
	move	$ref(input#1),x0	; 1st input -> x0
	move	$ref(input#2),a1	; 2nd input -> a1
	}

	codeblock(add,"int i") {
	add	x0,a	$ref(input#@i),x0
	}

	codeblock(addEnd) {
	add	x0,a1
	move	a1,$ref(output)
	}

	go {
		if (input.numberPorts() == 1) {
			addCode(copyInput);
			return;
		}

		addCode(addStart);
		for (int i = 3; i <= input.numberPorts(); i++) {
			addCode(add(i));
		}
		addCode(addEnd);
	}

	exectime {
		if ( input.numberPorts() == 1 ) return 2;
		return (2 + input.numberPorts());
	}
}
