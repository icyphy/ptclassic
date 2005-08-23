defstar {
	name { TableInt }
	domain { CGC }
	desc {
Implements an integer-valued lookup table.  The "values" state contains the
values to output; its first element is element zero.  An error occurs if
an out of bounds value is received.
	}
	version { @(#)CGCTableInt.pl	1.4	05/07/97 }
	author { Joseph T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	htmldoc {
<a name="table lookup"></a>
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
	}
	defstate {
		name { runTimeCheck }
		type { int }
		default { YES }
		desc { "If YES, check range of index at runtime, else omit"}
	}
	initCode {
		addInclude("<stdio.h>");
	}
	go {
		addCode(readIdx);
		if (int(runTimeCheck)) addCode(check);
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
