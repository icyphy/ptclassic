defstar {
	name { RectToCx }
	domain { CG56 }
	desc {Convert real and imaginary parts to a complex output.}
	version {$Id$}
	author { Jose Luis Pino }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	explanation {
.Id "real to complex format conversion"
.Id "format conversion, real to complex"
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
	codeblock (convert) {
	move 	$ref(real),x1
	move 	$ref(imag),x0
	move 	x,L:$addr(output)
	}
	go {
		addCode(convert);
	}
}
