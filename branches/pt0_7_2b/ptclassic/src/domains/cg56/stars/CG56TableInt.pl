defstar {
	name { TableInt }
	domain { CG56 }
	desc {
Implements an integer-valued lookup table.  The "values" state contains the
values to output; its first element is element zero.  An error occurs if
an out of bounds value is received.
	}
	version { $Id$ }
	author { Joseph T. Buck and Brian L. Evans }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	explanation {
.Id "table lookup"
	}
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	defstate {
		name { values }
		type { intarray }
		default { "-1 1" }
		desc { "table of values to output" }
		attributes { A_NONCONSTANT|A_YMEM|A_NOINIT }
	}
	defstate {
		name { runTimeCheck }
		type { int }
		default { YES }
		desc { "If YES, check range of index at runtime, else omit"}
	}
	constructor {
		noInternalState();
	}
	codeblock(readIdx) {
		move	#<$addr(input),a	; a = index into table
	}
	codeblock(check) {
; There is no check to make sure that the index is in bounds
	}
	codeblock(lookup) {
		move	$ref(values),x0
		add	x0,a
		move	a,r0
		move	y:(r0),$ref(output)
	}
	go {
		addCode(readIdx);
		if (int(runTimeCheck)) addCode(check);
		addCode(lookup);
	}
	exectime {
		return 5;
	}
}
