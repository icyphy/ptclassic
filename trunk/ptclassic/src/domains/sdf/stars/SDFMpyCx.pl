defstar {
	name {MpyCx}
	domain {SDF}
	desc { Output the product of the inputs, as a complex value.  }
	version {$Id$}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
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
		Complex product = 1.0;
		while ((p = nexti++) != 0)
			product *= Complex((*p)%0);
		output%0 << product;
	}
}
