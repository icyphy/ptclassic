defstar {
	name { DivByInt }
	domain { CGC }
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
	location { CGC main library }
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
	constructor {
		noInternalState();
	}
	setup {
		if ( int(divisor) == 0 ) {
		     Error::abortRun(*this, "divisor cannot be zero");
		}
	}
	codeblock(divide) {
	$ref(output) = $ref(input) / $val(divisor);
	}
	go {
		addCode(divide);
	}
}
