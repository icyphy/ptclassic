ident {
/************************************************************************
Version identification:
$Id$

Copyright (c) 1990 The Regents of the University of California.
                        All Rights Reserved.

Programmer:  D. G. Messerschmitt and E. A. Lee
Date of creation: 1/16/90
Modified to use preprocessor: 10/3/90, by EAL

Fork has a single input, and a multiple output. It copies its
input to each of the outputs.

Fork is generally used where it is desired to connect a Star
output to multiple other Star inputs

************************************************************************/
}
defstar {
	name {Fork}
	domain {SDF}
	desc { "A fork function: copies input particles to each output" }
	input {
		name{input}
		type{ANYTYPE}
	}
	outmulti {
		name{output}
		type{= input}
	}
	go {
		MPHIter nextp(output);
		PortHole* p;
		while ((p = nextp++) != 0)
			(*p)%0 = input%0;
	}
}

