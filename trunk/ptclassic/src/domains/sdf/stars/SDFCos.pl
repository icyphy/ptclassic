ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
			All Rights Reserved.

Programmer: J. T. Buck
Date of creation: 7/19/90
Converted to use preprocessor, 9/26/90

This star computes the cosine of its input, in radians.

************************************************************************/
}

defstar {
	name {Cos}
	domain {SDF}
	desc {
		"This star computes the cosine of its input, in radians."
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	go {
		output%0 << cos (input%0);
	}
}
