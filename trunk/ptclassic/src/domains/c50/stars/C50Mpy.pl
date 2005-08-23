defstar {
	name { Mpy }
	domain { C50 }
	desc { Multiply any number of inputs, producing an output. }
	version { @(#)C50Mpy.pl	1.4	01 Oct 1996 }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
<a name="multiplication"></a>
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

	codeblock (one) {
	lmmr	INDX,#$addr(input#1)		;just move data from in to out
	smmr	INDX,#$addr(output)
	}
	codeblock (std) {
	mar	*,AR7
	lar	AR7,#$addr(output)		;Address Output => AR7
	lmmr	INDX,#$addr(input#1)		;1st input -> INDX
	}
	codeblock(loop, "int i") {
	lmmr	TREG0,#$addr(input#@i)	;2(and following)input -> TREGO
	mpy     INDX				;Input#1 * Input#2
	sph     INDX				;result => INDX
	}
	codeblock (sat) {
	pac					;P-reg. => Accu
	sach	*,1				;Accu =>Output
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
			for (int i = 2; i <= input.numberPorts(); i++) {
				addCode(loop(i));
			}
			addCode(sat);
		}
	}
	exectime {
		if (input.numberPorts() == 1) return 2;
		else return input.numberPorts() + 10;
	}
}
