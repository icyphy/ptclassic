defstar {
	name { DivByInt }
	domain { CG56 }
	desc {
This is an amplifier; the integer output is the integer input
multiplied by the integer "gain" (default 1).
	}
	version { $Id$ }
	author { Brian L. Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	defstate {
		name { divisor }
		type { int }
		default { "1" }
		desc { Inverse of the gain of the amplifier. }
	}
	setup {
		if ( int(divisor) == 0 ) {
		     Error::abortRun(*this, "divisor cannot be zero");
		}
	}
	codeblock(copyInput) {
		move	$ref(input),x0
		move	x0,$ref(output)
	}
	codeblock(divideByTwo) {
		move	$ref(input),a
		asr	a
		move	a,$ref(output)
	}
	codeblock(divideByFour) {
		move	$ref(input),a
		asr	a
		asr	a
		move	a,$ref(output)
	}
	go {
		switch ( int(divisor) ) {
		  case 1:
		    addCode(copyInput);
		    break;
		  case 2:
		    addCode(divideByTwo);
		    break;
		  case 4:
		    addCode(divideByFour);
		    break;
		  default:
		    Error::abortRun(*this, "only supports integer division ",
				    "by 1, 2, or 4");
		    break;
		}
	}
}
