ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  E. A. Lee
 Date of creation: 9/16/90
 Converted to use preprocessor, 10/3/90, JTB

Convert magnitude and phase to rectangular form

**************************************************************************/
}
defstar {
	name { PolarRect }
	domain { SDF }
	desc {
		"Convert magnitude and phase to rectangular form"
	}
	input {
		name { magnitude }
		type { float }
	}
	input {
		name { phase }
		type { float }
	}
	output {
		name { x }
		type { float }
	}
	output {
		name { y }
		type { float }
	}
	ccinclude { <math.h> }
	go {
		float m = magnitude%0;
		float p = phase%0;
		x%0 << m * cos(p);
		y%0 << m * sin(p);
	}
}
