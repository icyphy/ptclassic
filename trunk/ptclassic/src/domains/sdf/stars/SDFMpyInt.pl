defstar {
	name {MpyInt}
	domain {SDF}
	desc { Output the product of the inputs, as an integer value. }
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	inmulti {
		name {input}
		type {int}
	}
	output {
		name {output}
		type {int}
	}
	go {
		MPHIter nexti(input);
		PortHole *p;
		int product = 1;
		while ((p = nexti++) != 0)
			product *= (int)((*p)%0);
		output%0 << product;
	}
}
