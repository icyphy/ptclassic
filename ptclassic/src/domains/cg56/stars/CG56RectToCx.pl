defstar {
	name { RectToCx }
	domain { CG56 }
	desc {Convert real and imaginary parts to a complex output.}
	version {$Id$}
	author { Jose Luis Pino }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="real to complex format conversion"></a>
<a name="format conversion, real to complex"></a>
	}
	input {
		name { real }
		type { FIX }
	}
	input {
		name { imag }
		type { FIX }
	}
	output {
		name { output }
		type { COMPLEX }
	}
	constructor {
		noInternalState();
	}
	codeblock (convert) {
	move 	$ref(real),x1
	move 	$ref(imag),x0
	move 	x,L:$addr(output)
	}
	go {
		addCode(convert);
	}
}