defstar {
	name { AddInt }
	domain { C50 }
	desc { Add any number of integer inputs, producing an integer output. }
	version { @(#)C50AddInt.pl	1.3	2/10/96 }
	author { Luis Gutierrez, based on CG56 version }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
The inputs are added and the result is written on the output.
The addition wraps up around to mimic the behavior of
two's complement addition.
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

	codeblock(saturate){
	setc	ovm
	}

	codeblock(copyInput) {
	ldp	#00h
	lmmr	ar0,#$addr(input)
	smmr	ar0,#$addr(output)
	}

	codeblock(addStart) {
	lar	ar0,#$addr(input#1)
	lar	ar1,#$addr(output)
	mar	*,ar0
	lacc	*,16
	lar	ar0,#$addr(input#2)
	}

	codeblock(add,"int i") {
	add	*,16
	lar	ar0,#$addr(input#@i)
	}

	codeblock(addEnd) {
	add	*,16,ar1
	sach	*
	}

	go {

		if (int(saturation)) addCodeblock(saturate);

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
		int time = 0;
		if ( input.numberPorts() == 1 ) retrun 3;
		if (int(saturation)){
			time ++;
		}
		time += 7;
		if (input.numberPorts() > 2){
		time += 2*(int(input.numberPorts()) - 2);
		}
		return time;
	}
}
