ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
			All Rights Reserved.

Programmer: E. A. Lee
Date of creation: 12/18/90

This star computes the signum of its input.
The output is +- 1.

************************************************************************/
}

defstar {
	name {Sgn}
	domain {SDF}
	desc {
		"This star computes the signum of its input."
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
		output%0 << copysign(1.0,(input%0));
	}
}
