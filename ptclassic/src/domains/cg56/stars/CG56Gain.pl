defstar {
	name { Gain }
	domain { CG56 }
	desc {
The output is set to the input multiplied by a gain term.  The gain must
be in [-1,1].
}
	version { $Id$ }
	author { J. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 arithmetic library }
	explanation {
We make no attempt to be heroic and handle all cases as was done with Gabriel.
The only special case is for gain 1.  We eliminate ourselves from the circuit
in that case.
	}
	protected {
		short identity;
	}
	start {
		identity = (double(gain) >= CG56_ONE);
		if (identity) forkInit(input,output);
	}
	execTime {
		return identity ? 0 : 5;
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
	mpyr	x1,y1,a
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
		} else if (double(gain)==0.0) {
		    addCode(cbZero);
		} else if (double(gain)==-1.0) {
		    addCode(cbNeg);
	 	} else {
		    addCode(cbStd);
		}
	}
}
