ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 7/19/90
 Converted to use preprocessor, 9/26/90

 This star produces a complex DC output (default zero)
 Even though we have ComplexState, the real and imaginary parts
 are separate states.  (Should this change?)

**************************************************************************/
}
defstar {
	name {ComplexDC}
	domain {SDF}
	desc {
		"Outputs a constant complex signal with real part 'real'\n"
		"(default 0.0) and imaginary part 'imag' (default 0.0)"
	}
	ccinclude { "ComplexSubset.h" }
	output {
		name {output}
		type {complex}
	}
	defstate {
		name {real}
		type{float}
		default {"0.0"}
		desc {"Real part of DC value"}
	}
	defstate {
		name {imag}
		type{float}
		default {"0.0"}
		desc {"Imaginary part of DC value"}
	}
	go {
		output%0 << Complex(real,imag);
	}
}

