defstar {
	name { RectToCx }
	domain { C50 }
	desc {Convert real and imaginary parts to a complex output.}
	version { $Id$ }
	author { Luis Gutierrez }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
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
	constructor {
		noInternalState();
	}
	codeblock (convert) {
	lmmr	ar0,#$addr(real)
	lmmr	ar1,#$addr(imag)
	smmr	ar0,#$addr(output,0)
	smmr	ar0,#$addr(output,1)
	}
	go {
		addCode(convert);
	}
	exectime{
		return 4;
	}
}
