ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
			All Rights Reserved.

Programmer: J. T. Buck
Date of creation: 7/19/90
Converted to use preprocessor, 10/3/90

This star computes the square root of its input.

************************************************************************/
}

defstar {
	name {Sqrt}
	domain {SDF}
	desc {
		"This star computes the square root of its input."
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	ccinclude { <math.h> }
	go {
		output%0 << sqrt (input%0);
	}
}

		
