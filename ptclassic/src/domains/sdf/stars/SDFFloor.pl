ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 10/8/90

 Returns the greatest integer <= the floating input.
**************************************************************************/
}
defstar {
	name { Floor }
	domain { SDF } 
	desc { 	"Outputs the greatest integer <= input." }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { int }
	}
	go {
		float t = input%0;
		int it = int(t);
		// handle rounding towards 0 on some processors
		if (it > t) it -= 1;
		output%0 << it;
	}
}
