defstar {
	name { TableInt }
	domain { C50 }
	desc {
Implements an integer lookup table.  The "values" state contains the
values to output; its first element is element zero.  An error occurs if
an out of bounds value is received.
	}
	version { $Id$ }
	author { Luis Gutierrez, based on CG56 version}
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
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
		desc { table of values to output }
		attributes { A_UMEM|A_CONSEC }
	}
	defstate {
		name { runTimeCheck }
		type { int }
		default { YES }
		desc {
If YES, check range of index at runtime, else omit.
This is currently not supported in the C50 domain.
		}
	}
	constructor {
		noInternalState();
	}
	codeblock(check) {
; There is no check to make sure that the index is in bounds
	}
	codeblock(lookup) {
	lacc	#$addr(values),0
	lar	ar0,#$addr(input)
	mar	*,ar0
	add	*
	samm	ar0
	bldd	*,#$addr(output)
	}
	go {
		if (int(runTimeCheck)) addCode(check);
		addCode(lookup);
	}
	exectime {
		return 6;
	}
}






