defstar {
	name { TableCx }
	domain { CGC }
	desc {
Implements a complex-valued lookup table.  The "values" state contains the
values to output; its first element is element zero.  An error occurs if
an out-of-bounds value is received.
	}
	version { $Id$ }
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
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
		type { complex }
	}
	defstate {
		name { values }
		type { complexarray }
		default { "(-1,0) (1,0)" }
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
		$ref(output).real = $ref2(values,idx).real;
		$ref(output).imag = $ref2(values,idx).imag;
	}
}
