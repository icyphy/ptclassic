ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 5/29/90
 Converted to use preprocessor, 10/3/90 

 Computes sum of n inputs.

**************************************************************************/
}

defstar {
	name {FloatSum}
	domain {SDF}
	desc {
		"Output the sum of the inputs, as a floating value"
	}
	inmulti {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {float}
	}
	go {
		MPHIter nexti(input);
		PortHole *p;
		double sum = 0.0;
		while ((p = nexti++) != 0)
			sum += float((*p)%0);
		output%0 << sum;
	}
}
