defstar {
    name { MpyCx }
    domain { C50 }
    desc {
Multiply any number of rectangular complex inputs, producing an output.
    }
    version { $Id$ }
    author { Luis Gutierrez }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
	type {COMPLEX}
    }
    output {
	name {output}
	type {COMPLEX}
    }
 
    constructor {
	noInternalState();
    }
    codeblock(cbZero) {
	zap
	samm	ar0
	smmr	ar0,#$addr(output)
    }
    codeblock(cbTwoInputs) {
	lar	ar0,#$addr(input#1)	; ar0-> first input
	lar	ar1,#$addr(input#2)	; ar1-> second input
	lar	ar2,#$addr(output)	; ar2-> output
	mar	*,ar0			; ar0->re1
	lt	*+,ar1			; treg0 = real1, ar0->imm1
	mpy	*+,ar0			; p = real1*real2; ar1->imm2
	ltp	*-,ar1			; acc = p, treg0 = imm1, ar0->re1
	mpy	*,ar0			; p = imm1*imm2, ar1->imm2
	lts	*+,ar2			; acc -= p, treg0 = re1,ar0->imm1
	sach	*+,1,ar1		; output real part of prod
	mpy	*-,ar0			; p = re1*imm2, ar1->re2
	ltp	*,ar1			; acc = p, treg0 = imm1
	mpy	*,ar2			; p = re2*imm1
	apac				; acc += p
	sach	*-,1		; output imaginary part of prod.; ar2->re(prod)
    }

    codeblock(cbMoreThanTwo,"int j"){
* for abbreviation po = previous output; inj = input j
	lar	ar0,#$addr(input#@j)	; load address of jth input, arp->ar2
	lt	*+,ar0			; treg0 = re(po), ar2->imm(po)
	mpy	*+,ar2			; p = re(inj)*re(po), r0->imm(inj)
	ltp	*-,ar0			; treg0 = imm(po), acc = p, r2->re(po)
	mpy	*,ar2			; p =imm(po)*imm(inj),ar0->imm(inj)
	lts	*			; acc -= p, treg0 = re(po)
	sach	*+,1,ar0		; store real, ar2->imm(po)
	mpy	*-,ar2		        ; p =re(po)*imm(inj),ar0->re(inj) 
	ltp	*,ar0			; acc = p, treg0=imm(po)
	mpy	*,ar2			; p = imm(po)*re(inj)
	apac				; acc += p
	sach	*-,1			; store imaginary
	}
 
   protected {
	int	runtime;
    }
    setup {
	int n = input.numberPorts();
	switch ( n ) {
	case 0:
	    runtime = 3;
	    break;
	case 1:
            {    
	    const char* buf = "input#1";;
	    CGPortHole *pr = (CGPortHole*) genPortWithName(buf);
	    forkInit( *pr, output);
	    runtime = 0;
	    break;
	    }
	case 2:
	    runtime = 15;
	    break;
	default:
	    runtime = 15 + (n-2)*12;
	    return;
	}
    }
    go {
	int n = input.numberPorts();
	if (n == 0) addCode(cbZero);
	if (n >= 2) addCode(cbTwoInputs);
	for (int i = 3; i <= n; i++)
		addCode(cbMoreThanTwo(i));
    }
    
    exectime {
        return runtime;
    }
}
