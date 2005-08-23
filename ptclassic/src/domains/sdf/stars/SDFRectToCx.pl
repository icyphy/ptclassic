defstar {
	name { RectToCx }
	domain { SDF }
	desc { Convert "real" and "imag" inputs to a complex output. }
	version {@(#)SDFRectToCx.pl	1.11 10/01/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	htmldoc {
<a name="real to complex format conversion"></a>
<a name="format conversion, real to complex"></a>
	}
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
	go {
		double r = real%0;
		double i = imag%0;
		output%0 << Complex(r,i);
	}
}
