ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 10/8/90

 Output is exp of input.
 Overflow should be handled better.

**************************************************************************/
}
defstar {
	name { Exp }
	domain { SDF } 
	desc { 	"Outputs exp of input" }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	ccinclude { <math.h> }
	go {
		float t = input%0;
		output%0 << exp(t);
	}
}
