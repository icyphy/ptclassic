defstar {
	name {Mpy}
	domain {SDF}
	desc { Output the product of the inputs, as a floating-point value. }
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
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
		double product = 1.0;
		while ((p = nexti++) != 0)
			product *= (double)((*p)%0);
		output%0 << product;
	}
}
