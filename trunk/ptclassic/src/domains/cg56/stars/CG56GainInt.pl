defstar {
	name { GainInt }
	domain { CG56 }
	desc {
The output is set to the input multiplied by a gain term.
}
	version { @(#)CG56GainInt.pl	1.5	06 Oct 1996 }
	author { Joseph T. Buck and Brian L. Evans }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
We handle the special cases for values of <i>gain</i> equal to
-1, 0, 1, 2, 3, 4, and 5.
In the case that the <i>gain</i> is one, we eliminate ourselves from
the circuit.
	}
	protected {
		int identity;
	}
	input {
		name {input}
		type {int}
	}
	output {
		name {output}
		type {int}
	}
	defstate {
		name {gain}
		type {int}
		default {2}
		desc {Gain value}
		attributes { A_XMEM|A_CONSTANT }
	}
	constructor {
		noInternalState();
	}
	setup {
		identity = (int(gain) == 1);
		if (identity) forkInit(input,output);
	}
	codeblock(cbNeg) {
	move	$ref(input),a			; input
	neg	a			; negate input
	move	a,$ref(output)
	}
	codeblock(cbZero) {
	clr	a				; gain of 0
	move	a,$ref(output)
	}
	codeblock(cbOne) {
; A gain of one has been removed from the system
	}
	codeblock(cbTwo) {
	move	$ref(input),a			; input
	asl	a				; multiply by 2
	move	a,$ref(output)
	}
	codeblock(cbThree) {
	move	$ref(input),a			; input
	asl	a	a,x0		; multiply by 3: 2*input + input 
	add	x0,a
	move	a,$ref(output)
	}
	codeblock(cbFour) {
	move	$ref(input),a			; input
	asl	a			; multiply by 4
	asl	a
	move	a,$ref(output)
	}
	codeblock(cbFive) {
	move	$ref(input),a			; input
	asl	a	a,b		; multiply by 5: 4*input + input
	addl	b,a
	move	a,$ref(output)
	}
	codeblock (cbStd) {
	move	$ref(input),x1			; input
	move	$ref(gain),y1			; gain
	mpy	x1,y1,a
	asr	a			; adjust for integer multiplication
	move	a0,$ref(output)
	}
	go {
		switch(int(gain)) {
		  case -1:
		    addCode(cbNeg);
		    break;
		  case 0:
		    addCode(cbZero);
		    break;
		  case 1:
		    addCode(cbOne);
		    break;
		  case 2:
		    addCode(cbTwo);
		    break;
		  case 3:
		    addCode(cbThree);
		    break;
		  case 4:
		    addCode(cbFour);
		    break;
		  case 5:
		    addCode(cbFive);
		    break;
		  default:
		    addCode(cbStd);
		    break;
		}
	}
	execTime {
		int pairOfIntervals = 0;
		switch(int(gain)) {
		  case -1:
		    pairOfIntervals = 3;
		    break;
		  case 0:
		    pairOfIntervals = 2;
		    break;
		  case 1:
		    pairOfIntervals = 0;
		    break;
		  case 2:
		    pairOfIntervals = 3;
		    break;
		  case 3:
		  case 4:
		  case 5:
		    pairOfIntervals = 4;
		    break;
		  default:
		    pairOfIntervals = 5;
		    break;
		}
		return pairOfIntervals;
	}
}
