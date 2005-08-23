defstar {
	name { GainInt }
	domain { C50 }
	desc {
The output is set to the input multiplied by a gain term.
	}
	version { @(#)C50GainInt.pl	1.8	06 Oct 1996  }
	author { Luis Gutierrez, based on CG56 version }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
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
		attributes { A_CONSTANT }
	}
 
	constructor {
		noInternalState();
	}
	setup {
		identity = (int(gain) == 1);
		if (identity){
			 output.setAttributes(P_NOINIT);
			 forkInit(input,output);
		}
	}
	codeblock(cbNeg) {
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	mar	*,ar0
	lacc	*,16,ar1
	setc	ovm
	neg
	sach	*,0
	clrc	ovm
	}

	codeblock(cbOne) {
* a gain of one has been removed from system
	}

	codeblock(cbPowTwo,"int shift"){
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	mar	*,ar0
	lacc	*,@shift,ar1
	sacl	*
	}

	codeblock(cbZero){
	zap
	lar	ar0,#$addr(output)
	mar	*,ar0
	sach	*
	}
	
	codeblock(cbPowTwoPlusOne,"int shift"){
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	mar	*,ar0
	lacc	*,@shift
	add	*,0,ar1
	sacl	*
	}
	
	codeblock(cbStd){
	lar	ar0,#$addr(input)
	lar	ar1,#$addr(output)
	lacc	#$val(gain),0
	samm	treg0
	mar	*,ar0
	mpy	*,ar1
	spl	*
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
		    addCode(cbPowTwo(1));
		    break;
		  case 3:
		    addCode(cbPowTwoPlusOne(1));
		    break;
		  case 4:
		    addCode(cbPowTwo(2));
		    break;
		  case 5:
		    addCode(cbPowTwoPlusOne(2));
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
		    pairOfIntervals = 7;
		    break;
		  case 0:
		    pairOfIntervals = 4;
		    break;
		  case 1:
		  case 3:
		  case 5:
		    pairOfIntervals = 6;
		    break;
		  case 2:
		  case 4:
		    pairOfIntervals = 5;
		    break;
		  default:
		    pairOfIntervals = 7;
		    break;
		}
		return pairOfIntervals;
	}
}
