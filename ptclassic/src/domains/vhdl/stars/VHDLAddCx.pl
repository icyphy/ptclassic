defstar {
	name { AddCx }
	domain { VHDL }
	desc { Output the sum of the inputs, as a complex value.  }
	version { $Id$ }
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	inmulti {
		name { input }
		type { complex }
	}
	output {
		name { output }
		type { complex }
	}
	constructor {
		noInternalState();
	}
	codeblock (std) {
$refCx(output, real) $assign(output) $interOp(+, input, real);
$refCx(output, imag) $assign(output) $interOp(+, input, imag);
	}
	go {
	  addCode(std);
	}
}
