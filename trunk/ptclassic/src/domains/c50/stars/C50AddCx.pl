defstar {
	name { AddCx }
	domain { C50 }
	desc { Add any number of integer inputs, producing an integer output. }
	version { $Id$ }
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
		type {COMPLEX}
	}
	output {
		name {output}
		type {COMPLEX}
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
	lmmr	ar0,#$addr(input,0)
	smmr	ar0,#$addr(output,0)
	lmmr	ar0,#$addr(input,1)
	smmr	ar0,#$addr(output,1)
	}

	codeblock(addStart) {
	lar	ar0,#$addr(input#1)
	lar	ar1,#$addr(output)
	lar	ar2,#$addr(input#2)
	mar	*,ar0
	lacc	*+,16,ar2			; acc = re(in1)
	add	*+,16,ar1			; acc =re(in1)+re(in2)
	sach	*+,0,ar0			; store real result
	lacc	*,16,ar2			; acc = imm(in1)
	add	*,16,ar1			; acc =imm(in1)+imm(in2)
	sach	*,0,r0				; store imaginary result
	}


//addOdd assumes that imaginary part of prev. result is stored in 
//acc and that arp -> ar0
	codeblock(addOdd,"int j"){
	lar	ar0,#$addr(input#@j,1)		; ar0 -> imm(inj)
	add	*-,16,ar1			;acc=imm(inj)+imm(po),
	sach	*-,0,ar0
	lacc	*,16,ar1			;
	add	*,16,ar0
	}
//addEven assumes that real part of prev result is stored in acc
// and that arp-> ar0
	codeblock(addEven,"int j"){
	lar	ar0,#$addr(input#@j,0)
	add	*+,16,ar1
	sach	*+,0,ar0
	lacc	*,16,ar1
	add	*,16,ar0
	}

	codeblock(addEnd){
	mar	*,ar1
	sach	*,0
	}


	go {

		if (int(saturation)) addCode(saturate);

		if (input.numberPorts() == 1) {
			addCode(copyInput);
			return;
		}
		int even;
		addCode(addStart);
		even = 0;
		for (int i = 3; i <= input.numberPorts(); i++) {
			if (even) {
				addCode(addEven(i));
				even = 0;
			} else {
				addCode(addOdd(i));
				even = 1;
			}
		}
		if (input.numberPorts() > 2) addCode(addEnd);
	}

	exectime {
		int time = 0;
		if ( input.numberPorts() == 1 ) return 4;
		if (int(saturation)){
			time ++;
		}
		time += 10;
		if (input.numberPorts() > 2){
		time += 5*(int(input.numberPorts()) - 2) + 2;
		}
		return time;
	}
}



