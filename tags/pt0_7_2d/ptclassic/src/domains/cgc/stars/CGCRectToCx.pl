defstar {
	name { RectToCx }
	domain { CGC }
	desc {Convert real and imaginary parts to a complex output.}
	version { @(#)CGCRectToCx.pl	1.6	1/1/96 }
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
	codeblock(body) {
	$ref(output).real = $ref(real);
	$ref(output).imag = $ref(imag);
	}
	
	go {
		addCode(body);
	}
	exectime {
		return 2;
	}
}
