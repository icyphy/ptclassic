defstar {
	name { SubCx }
	domain { C50 }
	desc { output is the "pos" input minus all the "neg" inputs }
	version {@(#)C50SubCx.pl	1.7	05/26/98}
	author { Luis Gutierrez, based on CG56 version, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
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


	codeblock(clearSaturation){
	clrc	ovm
	}
	
	codeblock(subStart) {
	lar	ar0,#$addr(pos)			; ar0->pos input
	lar	ar1,#$addr(output)		; ar1-> output
	lar	ar2,#$addr(neg#1)		; ar2-> neg input 1
	mar	*,ar0				; arp = 0
	lacc	*+,16,ar2			; acc = re(pos); ar0->im(pos)	
	sub	*+,16,ar1			; acc =re(pos)-re(neg1); ar2->im(neg1)
	sach	*+,0,ar0			; store real result;ar1->im(out)
	lacc	*,16,ar2			; acc = imm(pos)
	sub	*,16,ar1			; acc =imm(pos)-imm(neg1)
	sach	*,0,ar0				; store imaginary result
	}


//subOdd assumes that imaginary part of prev. result is stored in 
//acc and that arp -> ar0; ar0->im(inj); ar1->im(po)
	codeblock(subOdd,"int j"){
	lar	ar0,#($addr(neg#@j)+1)	    ; ar0 -> imm(inj)
	sub	*-,16,ar1		    ;acc=imm(po)-imm(inj); ar0->re(inj)
	sach	*-,0,ar1		    ;im(out) = acc; ar1->re(po)
	lacc	*,16,ar0		    ;acc = re(po)
	sub	*,16,ar0       ;acc = re(po)-re(inj); ar0->re(inj), ar1->re(po)
	}
//subEven assumes that real part of prev result is stored in acc
// and that arp-> ar0; 
	codeblock(subEven,"int j"){
	lar	ar0,#$addr(neg#@j)	    ;ar0->re(inj)		
	sub	*+,16,ar1		    ;acc = re(po)-re(inj); ar0->im(inj)
	sach	*+,0,ar1		    ;re(out) = acc; ar1->im(po)
	lacc	*,16,ar0		    ;acc = im(po)
	sub	*,16,ar0     ;acc = im(po) - im(inj); ar0->im(inj); ar1->im(po)
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

		if (int(saturation)) addCode(clearSaturation);
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



