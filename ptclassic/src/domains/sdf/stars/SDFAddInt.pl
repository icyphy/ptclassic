defstar {
	name {AddInt}
	domain {SDF}
	desc { Output the sum of the inputs, as an integer.  }
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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
		int sum = 0;
		while ((p = nexti++) != 0)
			sum += int((*p)%0);
		output%0 << sum;
	}
}
