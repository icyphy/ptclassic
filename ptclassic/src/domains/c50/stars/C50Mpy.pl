defstar {
	name { Mpy }
	domain { C50 }
	desc { Multipy any number of inputs, producing an output. }
	version { $Id$ }
	author { A. Baensch, ported from Gabriel }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 arithmetic library }
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
	codeblock (one) {
	lmmr	INDX,#$addr(input#1)		; just move data from in to out
	smmr	INDX,#$addr(output)
	}
	codeblock (std) {
	mar	*,AR7
	lar	AR7,#$addr(output)		; Adress Output => AR7
	lmmr	INDX,#$addr(input#1)		; 1st input -> INDX
	}
        codeblock(loop) {
	lmmr	TREG0,#$addr(input#inputNum)	;2(and following)input -> TREGO
        mpy     INDX				;Input#1 * Input#2
	sph     INDX				;result => INDX
        }
	codeblock (sat) {
	pac					;P-reg. => Accu
	sach	*,1				;Accu =>Output
	}

	go {
		if (input.numberPorts() == 1) {
			addCode(one);
			return;
		}
		addCode(std);
		for (int i = 2; i <= input.numberPorts(); i++) {
	                inputNum=i;
			addCode(loop);
		}
	        addCode(sat);
	}
	exectime {
		if (input.numberPorts() == 1) return 2;
		else return input.numberPorts() + 10;
	}
}
