defstar {
	name { MpyInt }
	domain { C50 }
	desc {
Multiply any number of inputs producing an output.
	}
	version { @(#)C50MpyInt.pl	1.7	01 Oct 1996 }
	author { Luis Gutierrez, A. Baensch, ported from Gabriel }
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
	ldp	#00h
	lmmr	ar0,#$addr(input)
	smmr	ar0,#$addr(output)
	}

	codeblock (multiplyStart) {
	ldp	#00h			
	lmmr	treg0,#$addr(input#1)	; treg0 = first input
	lmmr	ar2,#$addr(input#2)	; ar2 = second input
	}


        codeblock(multiply,"int i") {
	mpy	ar2			; preg = treg * (ar2)
	spl	ar2			; (ar2) = preg
	lmmr	treg0,#$addr(input#@i)	; load treg0 with next input
        }

	codeblock (multiplyEnd) {
	mpy	ar2			
	spl	ar2
	smmr	ar2,#$addr(output)
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
		int time = 0;
		if (input.numberPorts() == 1)  return 3;
		time += 6;
		if (input.numberPorts() > 2)
			time += 3*(int(input.numberPorts()) - 2);
		return time;
	}
}

