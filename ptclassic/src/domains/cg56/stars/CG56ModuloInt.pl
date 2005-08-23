defstar {
	name {ModuloInt}
	domain {CG56}
	desc { 
The output is equal to the remainder after dividing the
integer input by the integer "modulo" parameter.
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
		name{input}
		type{int}
	}
	output {
		name{output}
		type{int}
	}
	defstate {
		name{modulo}
		type{int}
		default{"10"}
		desc {The modulo parameter}
	}
	defstate {
		name{powerOfTwoMask}
		type{int}
		default{"1"}
		desc {
Invisible parameter of the mask to implement integer modulo arithmetic
by powers of two.
		}
		attributes { A_NONCONSTANT|A_NONSETTABLE }
	}
	codeblock(powerOfTwo) {
	move	$ref(input),a
	move	$val(powerOfTwoMask),x0
	and	x0,a
	move	a,$ref(output)
	}
	setup {
		if ( int(modulo) <= 0 ) {
		    Error::abortRun(*this,
				    "The modulo parameter must be positive");
		    return;
		}

		// convert the modulo parameter to a mask (stop at 16 bits)
		int regValue = 0x01;		// 1, 2, 4, 8, ...
		int mask = 0x00;		// 0, 1, 3, 7, ...
		int modValue = int(modulo);
		for (int i = 0; i < 16; i++) {
		    if ( regValue == modValue ) break;
		    mask = (mask << 1) | 0x01;
		    regValue <<= 1;
		}

		if ( i == 16 ) {
		    Error::abortRun(*this,
				    "The modulo parameter must be positive");
		    return;
		}

		powerOfTwoMask = mask;
	}
	go {
		addCode(powerOfTwo);
	}
}
