ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 9/19/90
 Converted to use preprocessor, 9/26/90 

 Converts complex values to real and imaginary parts.

**************************************************************************/
}

defstar {
	name {ComplexReal}
	domain {SDF}
	desc {
		"Convert complex data to real and imaginary parts"
	}
	input {
		name {input}
		type {complex}
	}
	output {
		name {real}
		type {float}
	}
	output {
		name {imag}
		type {float}
	}
	go {
		Complex t = input%0;
		real%0 << t.real();
		imag%0 << t.imag();
	}
}
