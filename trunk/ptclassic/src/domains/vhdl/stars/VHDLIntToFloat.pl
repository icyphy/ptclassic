defstar {
	name { IntToFloat }
	domain { VHDL }
	desc { type conversion from int to float }
	version { $Id$ }
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { float }
	}
	constructor {
		noInternalState();
	}
	go {
	  StringList out;

	  out << "$ref(output) $assign(output) REAL ($ref(input));\n";

	  addCode(out);
	  out.initialize();
	}
}
