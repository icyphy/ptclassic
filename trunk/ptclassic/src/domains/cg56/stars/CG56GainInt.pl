defstar {
	name { GainInt }
	domain { CG56 }
	desc {
The output is set to the input multiplied by a gain term.
}
	version { $Id$ }
	author { Joseph T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	explanation {
We make no attempt to be heroic and handle all cases as was done with Gabriel.
The only special case is for a \fIgain\fR of 1.
We eliminate ourselves from the circuit in that case.
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
		default {"2"}
		desc {Gain value}
	}
	constructor {
		noInternalState();
	}
	setup {
		identity = (int(gain) == 1);
		if (identity) forkInit(input,output);
	}
	codeblock (cbStd) {
	move	$ref(input),x1
	move	#$val(gain),y1
	mpy	x1,y1,a
	move	a1,$ref(output)
	}
	codeblock(cbZero) {
	clr	a
	move	a,$ref(output)
	}
	codeblock(cbNeg) {
	move	$ref(input),a1
	neg	a
	move	a1,$ref(output)
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
		    // generate no code
		    break;
		  default:
		    addCode(cbNeg);
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
		  default:
		    pairOfIntervals = 4;
		    break;
		}
		return pairOfIntervals;
	}
}
