defstar {
	name { Gain }
	domain { C50 }
	desc {
The output is set to the input multiplied by a gain term.  The gain must
be in [-1,1].
}
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 arithmetic library }
	explanation {
We make no attempt to be heroic and handle all cases as was done with Gabriel.
The only special case is for gain 1.  We eliminate ourselves from the circuit
in that case.
	}
	protected {
		short identity;
	}
	setup {
		identity = (gain.asDouble() >= C50_ONE);
		if (identity) forkInit(input,output);
	}
	execTime {
		return identity ? 0 : 8;
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	defstate {
		name {gain}
		type {FIX}
		default {0.5}
		desc {Gain value}
		attributes { A_UMEM }
	}
	codeblock (cbStd) {
	mar 	*,AR6
	lar	AR0,#$addr(gain)		;Address gain		=>AR0
	lar	AR6,#$addr(input)		;Address input		=>AR6
	lar	AR7,#$addr(output)		;Address output		=>AR7
	lt	*,AR0				;TREG0 = input
	mpy	*,AR7				;input*gain
	pac					;Accu=input*gain
	sach	*,1				;output=input*gain
	}
	codeblock(cbZero) {
	splk	#0,AR5				;AR5 = zero
	smmr	AR5,#$addr(output)		;output =0
	}
	codeblock(cbNeg) {
	mar	*,AR6
	lar	AR6,#$addr(input)		;Address input		=>AR6
	lar	AR7,#$addr(output)		;Address output		=>AR7
	lacc	*,15,AR7			;Accu = input
	neg					;Accu = -input
	sach	*,1				;output = -input
	}
	go {
		if (identity) {
		    ;
		} else if (gain.asDouble()==0.0) {
		    addCode(cbZero);
		} else if (gain.asDouble()==-1.0) {
		    addCode(cbNeg);
	 	} else {
		    addCode(cbStd);
		}
	}
}





