defstar {
	name { Table }
	domain { CGC }
	desc {
Implements a real-valued lookup table.  The "values" state contains the
values to output; its first element is element zero.  An error occurs if
an out of bounds value is received.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
.Id "table lookup"
	}
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { values }
		type { floatarray }
		default { "-1 1" }
		desc { "table of values to output" }
	}
	defstate {
		name { runTimeCheck }
		type { int }
		default { YES }
		desc { "If YES, check range of index at runtime, else omit"}
	}
	go {
		addCode(readIdx);
		if (runTimeCheck) addCode(check);
		addCode(lookup);
	}
	constructor {
		noInternalState();
	}
	codeblock(readIdx) {
		int idx = $ref(input);
	}
	codeblock(check) {
		if (idx < 0 || idx >= $size(values)) {
			fprintf(stderr, "ERROR: input to CGCTable out of range\n");
			exit(1);
		}
	}
	codeblock(lookup) {
		$ref(output) = $ref2(values,idx);
	}
}

