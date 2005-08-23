defstar {
	name { AddInt }
	domain { C50 }
	desc { Add any number of integer inputs, producing an integer output. }
	version { @(#)C50AddInt.pl	1.7	01 Oct 1996 }
	author { Luis Gutierrez, based on CG56 version }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
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


	codeblock(clearSaturation){
	clrc	ovm
	}

	codeblock(copyInput) {
	lmmr	ar0,#$addr(input)
	smmr	ar0,#$addr(output)
	}

	codeblock(addStart) {
	lar	ar0,#$addr(input#1)	;ar0->first input
	lar	ar1,#$addr(output)	;ar1->output
	mar	*,ar0			;arp = 0
	lacc	*,16			;accH = first input
	lar	ar0,#$addr(input#2)	;ar0 -> second input
	}



	codeblock(add,"int i") {
	add	*,16			; add last input pointed to
	lar	ar0,#$addr(input#@i)	; point to next input
	}

	codeblock(addEnd) {
	add	*,16,ar1
	sach	*
	}

	go {

		if (int(saturation)) addCode(saturate);

		if (input.numberPorts() == 1) {
			addCode(copyInput);
			return;
		}

		addCode(addStart);
		for (int i = 3; i <= input.numberPorts(); i++) {
			addCode(add(i));
		}
		addCode(addEnd);
		if (int(saturation)) addCode(clearSaturation);
	}

	exectime {
		int time = 0;
		if ( input.numberPorts() == 1 ) return 2;
		if (int(saturation)){
			time += 2;
		}
		time += 7;
		if (input.numberPorts() > 2){
		time += 2*(int(input.numberPorts()) - 2);
		}
		return time;
	}
}
