defstar {
	name { AddInt }
	domain { VHDL }
	desc { Output the sum of the inputs, as a floating value.  }
	version { @(#)VHDLAddInt.pl	1.3 03/07/96 }
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
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	constructor {
		noInternalState();
	}
	codeblock (std) {
$ref(output) $assign(output) $interOp(+, input);
	}
	go {
	  addCode(std);
	}
}
