defstar {
	name { Gain }
	domain { VHDL }
	desc {
	}
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
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { gain }
		type { float }
		default { 1.0 }
	}
	codeblock (std) {
$ref(output) $assign(output) $ref(gain) * $ref(input);
	}
	go {
	  addCode(std);
	}
}
