defstar {
	name { TableCx }
	domain { SDF }
	desc {
Implements a complex-valued lookup table.  The "values" state contains
the values to output; its first element is element zero.  An error occurs
if an out of bounds value is received.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
.Id "table lookup, complex"
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
		default { "(1, 0) (0, 1) (-1, 0) (0, -1)" }
		desc { "table of values to output" }
	}
	go {
		int i = int(input%0);
		if (i < 0 || i > values.size())
			Error::abortRun(*this, ": out of bounds input value");
		else output%0 << values[i];
	}
}
