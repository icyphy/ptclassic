ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 9/19/90
 Converted to use preprocessor, 10/3/90, JTB

Convert real and imaginary parts to a complex value

**************************************************************************/
}
defstar {
	name { RealComplex }
	domain { SDF }
	desc {"Convert real and imaginary parts to a complex output"}
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
