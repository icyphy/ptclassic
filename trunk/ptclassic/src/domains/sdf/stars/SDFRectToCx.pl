defstar {
	name { RectToCx }
	domain { SDF }
	desc {Convert real and imaginary parts to a complex output.}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
.Id "real to complex format conversion"
.Id "format conversion, real to complex"
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
