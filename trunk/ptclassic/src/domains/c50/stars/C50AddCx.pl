defstar {
	name { AddCx }
	domain { C50 }
	desc { Add any number of integer inputs, producing an integer output. }
	version {@(#)C50AddCx.pl	1.8	05/26/98}
	author { Luis Gutierrez, based on CG56 version, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
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


	codeblock(clearSaturation){
	clrc	ovm
	}

	codeblock(copyInput) {
	lmmr	ar0,#$addr(input)
	smmr	ar0,#$addr(output)
	lmmr	ar0,#($addr(input)+1)
	smmr	ar0,#($addr(output)+1)
	}

	codeblock(addStart) {
	lar	ar0,#$addr(input)		; ar0-> input 1
	lar	ar1,#$addr(output)		; ar1-> output
	lar	ar2,#($addr(input)+1)		; ar2-> input2
	mar	*,ar0				; arp = 0
	lacc	*+,16,ar2			; acc = re(in1); ar0->im(in1)
	add	*+,16,ar1			; acc =re(in1)+re(in2); ar2->im(in2)
	sach	*+,0,ar0			; store real result; ar1->im(out)
	lacc	*,16,ar2			; acc = imm(in1); ar0->im(in1)
	add	*,16,ar1			; acc =imm(in1)+imm(in2); ar2->im(in2)
	sach	*,0,ar0				; store imaginary result; ar1->im(out)}



//addOdd assumes that imaginary part of prev. result is stored in 
//acc and that arp -> ar0; ar0->im(inj); ar1->im(po)
	codeblock(addOdd,"int j"){
	lar	ar0,#($addr(input#@j)+1)		; ar0 -> imm(inj)
	add	*-,16,ar1			;acc=imm(inj)+imm(po);ar0->re(inj)
	sach	*-,0,ar0			;imm(out) = acc; ar1->re(po)
	lacc	*,16,ar1			;acc = re(inj); ar0->re(inj)
	add	*,16,ar0			; acc = re(inj)+re(po); ar1->re(po)
	}
//addEven assumes that real part of prev result is stored in acc
// and that arp-> ar0; ar0->re(inj); ar1->re(po)
	codeblock(addEven,"int j"){
	lar	ar0,#$addr(input#@j)		; ar0->re(inj)
	add	*+,16,ar1			; acc = re(inj)+re(po); ar0->im(inj)
	sach	*+,0,ar0			; re(out) = acc; ar1-> im(po)
	lacc	*,16,ar1			; acc = im(j); ar0->im(inj)
	add	*,16,ar0			; acc = im(inj)+im(po)
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
		if (int(saturation)) addCode(clearSaturation);
	}

	exectime {
		int time = 0;
		if ( input.numberPorts() == 1 ) return 4;
		if (int(saturation)){
			time += 2;
		}
		time += 10;
		if (input.numberPorts() > 2){
		time += 5*(int(input.numberPorts()) - 2) + 2;
		}
		return time;
	}
}



