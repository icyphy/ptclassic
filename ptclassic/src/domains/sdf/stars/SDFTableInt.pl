defstar {
	name { TableInt }
	domain { SDF }
	desc {
This star implements a integer-valued lookup table indexed by an
integer-valued input.  The input must lie between 0 and N-1,
inclusive, where N is the size of the table.  The "values" parameter
specifies the table.  Its first element is indexed by a zero-valued
input. An error occurs if the input value is out of bounds.
	}
	version {@(#)SDFTableInt.pl	1.10	01 Oct 1996}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
<a name="table lookup, integer"></a>
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
		if (i < 0 || i >= values.size()) {
			StringList msg = "input index  ";
			msg << i << " is out of bounds for the table";
			Error::abortRun(*this, msg);
			return;
		}
		output%0 << values[i];
	}
}
