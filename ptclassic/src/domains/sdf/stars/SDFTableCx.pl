defstar {
	name { TableCx }
	domain { SDF }
	desc {
This star implements a complex-valued lookup table indexed by an
integer-valued input.  The input must lie between 0 and N-1,
inclusive, where N is the size of the table. The "values" parameter
specifies the table.  Its first element is indexed by a zero-valued
input. An error occurs if the input value is out of bounds.
	}
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
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
		if (i < 0 || i >= values.size())
			Error::abortRun(*this, ": input index is out of bounds for the table");
		else
			output%0 << values[i];
	}
}
