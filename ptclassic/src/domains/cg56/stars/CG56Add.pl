defstar {
	name { Add }
	domain { CG56 }
	desc { Add any number of inputs, producing an output. }
	version { $Id$ }
	author { J. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 demo library }
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
	state {
		name { saturation }
		type { int }
		default { "YES" }
		desc { If true, use saturation arithmetic }
	}
	codeblock (std) {
	move	$ref(input#1),x0	; 1st input -> x0
	move	$ref(input#2),a		; 2nd input -> a
	}
	codeblock (sat) {
	add	x0,a
	move	a,$ref(output)		; this move saturates
	}
	codeblock (nosat) {
	add	x0,a
	move	a1,$ref(output)		; this move clips
	}
	codeblock (one) {
	move	$ref(input#1),x0	; just move data from in to out
	move	x0,$ref(output)
	}
	go {
		if (input.numberPorts() == 1) {
			addCode(one);
			return;
		}
		addCode(std);
		for (int i = 3; i <= input.numberPorts(); i++) {
			char buf[80];
			sprintf (buf, "\tadd\tx0,a\t$ref(input#%d),x0",i);
			addCode(CodeBlock(buf));
		}
		if (int(saturation))
			addCode(sat);
		else addCode(nosat);
	}
	exectime {
		if (input.numberPorts() == 1) return 2;
		else return input.numberPorts() + 2;
	}
}
