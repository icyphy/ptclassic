defstar {
	name { RectToCx }
	domain { VHDL }
	desc { Convert real and imaginary parts to a complex output. }
	version { $Id$ }
	author { Michael C. Williamson, S. Ha }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	input {
		name { real }
		type { float }
	}
	input {
		name { imag }
		type { float }
	}
	output {
		name { output }
		type { complex }
	}
	constructor {
		noInternalState();
	}
	codeblock (std) {
$refCx(output,real) $assign(output) $ref(real);
$refCx(output,imag) $assign(output) $ref(imag);
	}
	
	go {
		addCode(std);
	}
}
