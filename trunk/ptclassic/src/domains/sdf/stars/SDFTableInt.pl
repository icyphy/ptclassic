defstar {
	name { TableInt }
	domain { SDF }
	desc {
This star implements a real-valued lookup table indexed by an integer-valued input.
The input must lie between 0 and N-1, inclusive, where N is the size of the table
The "values" parameter specifies the table.  Its first element is indexed by a
zero-valued input. An error occurs if the input value is out of bounds.
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
.Id "table lookup, integer"
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
	go {
		int i = int(input%0);
		if (i < 0 || i > values.size())
			Error::abortRun(*this, ": out of bounds input value");
		else output%0 << values[i];
	}
}
