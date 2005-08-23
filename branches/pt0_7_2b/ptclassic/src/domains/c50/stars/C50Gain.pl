defstar {
	name { Gain }
	domain { C50 }
	desc {
The output is set to the input multiplied by a gain term.
The gain must be in interval [-1,1].
	}
	version { @(#)C50Gain.pl	1.4	01 Oct 1996 }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
We make no attempt to be heroic and handle all cases as was done with Gabriel.
The only special case is for gain 1.  We eliminate ourselves from the circuit
in that case.
	}
	protected {
		short identity;
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
	constructor {
		noInternalState();
	}
	setup {
		double thegain = gain.asDouble();
		if (thegain < -1.0 || thegain > 1.0) {
			Error::warn(*this, "gain is not in the range [-1,1]");
		}
		identity = (thegain >= C50_ONE);
		if (identity) forkInit(input, output);
	}
	go {
		if (identity) {
		    ;
		} else if (gain.asDouble() == 0.0) {
		    addCode(cbZero);
		} else if (gain.asDouble() == -1.0) {
		    addCode(cbNeg);
	 	} else {
		    addCode(cbStd);
		}
	}
	execTime {
		identity = (gain.asDouble() >= C50_ONE);
		return identity ? 0 : 8;
	}
}
