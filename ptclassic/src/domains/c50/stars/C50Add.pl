defstar {
	name { Add }
	domain { C50 }
	desc { Add any number of inputs, producing an output. }
	version { @(#)C50Add.pl	1.5	01 Oct 1996 }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
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


	codeblock(sat){
	setc	ovm
	}

	codeblock (std) {
	mar	*,AR5
	lar	AR5,#$addr(input#2)	; address 2nd input -> AR5
	lar	AR6,#$addr(input#1)	; address 1st input -> AR6
	lar	AR7,#$addr(output)
	lacc	*,16,AR5		; 2nd input -> ACC
	}
	codeblock (loop, "int i") {
	lar	AR5,#$addr(input#@i)
	add	*,16
	}
	codeblock (end) {
	mar	*,AR6
	add	*,16,AR7		; add 1st input
	sach	*,0			; result -> output
	}

	codeblock(clearSat){
	clrc	ovm
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
			if (int(saturation)) addCode(sat);
			addCode(std);
			for (int i = 3; i <= input.numberPorts(); i++) {
				addCode(loop(i));
			}
	 		addCode(end);
			if (int(saturation)) addCode(clearSat);
		}
	}
	exectime {
		int time; 
		if (input.numberPorts() == 1) time = 4;
		else time = input.numberPorts() + 6;
		if (int(saturation)) time += 2;
		return time;
	}
}
