ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 5/29/90
 Converted to use preprocessor, 9/26/90 

 Computes complex sum of n inputs.

**************************************************************************/
}

defstar {
	name {ComplexSum}
	domain {SDF}
	desc {
		"Output the sum of the inputs, as a complex value"
	}
	inmulti {
		name {input}
		type {complex}
	}
	output {
		name {output}
		type {complex}
	}
	go {
		MPHIter nexti(input);
		PortHole *p;
		Complex sum = 0.0;
		while ((p = nexti++) != 0)
			sum += Complex((*p)%0);
		output%0 << sum;
	}
}
