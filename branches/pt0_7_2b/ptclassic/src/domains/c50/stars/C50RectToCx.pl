defstar {
	name { RectToCx }
	domain { C50 }
	desc {Convert real and imaginary parts to a complex output.}
	version {@(#)C50RectToCx.pl	1.5	05/26/98}
	author { Luis Gutierrez, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
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
	lmmr	ar0,#$addr(real)
	lmmr	ar1,#$addr(imag)
	smmr	ar0,#$addr(output)
	smmr	ar0,#($addr(output)+1)
	}
	go {
		addCode(convert);
	}
	exectime{
		return 4;
	}
}
