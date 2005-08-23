defstar {
	name { Mpy }
	domain { VHDL }
	desc { Output the product of the inputs, as a floating value.  }
	version { @(#)VHDLMpy.pl	1.2 03/07/96 }
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	inmulti {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	constructor {
		noInternalState();
	}
	codeblock (std) {
$ref(output) $assign(output) $interOp(*, input);
	}
	go {
	  addCode(std);
	}
	exectime {
		return input.numberPorts();
	}
}
