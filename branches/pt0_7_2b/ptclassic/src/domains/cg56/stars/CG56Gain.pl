defstar {
	name { Gain }
	domain { CG56 }
	desc {
The output is set to the input multiplied by a gain term.
The gain must be in the interval [-1,1].
	}
	version { @(#)CG56Gain.pl	1.21 06 Oct 1996 }
	author { J. Buck }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
We make no attempt to be heroic and handle all cases as was done with Gabriel.
The only special case is for a <i>gain</i> of 1.
We eliminate ourselves from the circuit in that case.
	}
	protected {
		short identity;
	}
	setup {
		double thegain = gain.asDouble();
		if (thegain < -1.0 || thegain > 1.0) {
		    Error::warn(*this, "gain is not in the range [-1,1]");
		}
		identity = (thegain >= CG56_ONE);
		if (identity) forkInit(input, output);
	}
	constructor {
		noInternalState();
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
	}
	codeblock (cbStd) {
	move	$ref(input),x1
	move	#$val(gain),y1
	mpyr	y1,x1,a
	move	a,$ref(output)
	}
	codeblock(cbZero) {
	clr	a
	move	a,$ref(output)
	}
	codeblock(cbNeg) {
	move	$ref(input),a
	neg	a
	move	a,$ref(output)
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
		identity = (gain.asDouble() >= CG56_ONE);
		return identity ? 0 : 5;
	}
}
