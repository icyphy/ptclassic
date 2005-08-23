defstar {
	name { SubCx }
	domain { C50 }
	desc { output is the "pos" input minus all the "neg" inputs }
	version { $Id$ }
	author { Luis Gutierrez, based on CG56 version }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }

	input{
		name{pos}
		type{COMPLEX}
	}

	inmulti {
		name {neg}
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

	codeblock(subStart) {
	lar	ar0,#$addr(pos)
	lar	ar1,#$addr(output)
	lar	ar2,#$addr(neg#1)
	mar	*,ar0
	lacc	*+,16,ar2			; acc = re(in1)
	sub	*+,16,ar1			; acc =re(in1)+re(in2)
	sach	*+,0,ar0			; store real result
	lacc	*,16,ar2			; acc = imm(in1)
	sub	*,16,ar1			; acc =imm(in1)-imm(in2)
	sach	*,0,r0				; store imaginary result
	}


//subOdd assumes that imaginary part of prev. result is stored in 
//acc and that arp -> ar0
	codeblock(subOdd,"int j"){
	lar	ar0,#$addr(neg#@j,1)		; ar0 -> imm(inj)
	sub	*-,16,ar1			;acc=imm(inj)-imm(po),
	sach	*-,0,ar0
	lacc	*,16,ar1			;
	sub	*,16,ar0
	}
//subEven assumes that real part of prev result is stored in acc
// and that arp-> ar0
	codeblock(subEven,"int j"){
	lar	ar0,#$addr(neg#@j,0)
	sub	*+,16,ar1
	sach	*+,0,ar0
	lacc	*,16,ar1
	sub	*,16,ar0
	}

	codeblock(subEnd){
	mar	*,ar1
	sach	*,0
	}


	go {

		if (int(saturation)) addCode(saturate);

		int even;
		addCode(subStart);
		even = 0;
		for (int i = 2; i <= neg.numberPorts(); i++) {
			if (even) {
				addCode(subEven(i));
				even = 0;
			} else {
				addCode(subOdd(i));
				even = 1;
			}
		}
		if (neg.numberPorts() > 1) addCode(subEnd);
	}

	exectime {
		int time = 0;
		time += 10;
		if (neg.numberPorts() > 1){
		time += 5*(int(neg.numberPorts()) - 1) + 2;
		}
		return time;
	}
}



