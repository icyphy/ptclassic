defstar {
	name {Add}
	domain {SDF}
	desc { Output the sum of the inputs, as a floating value.  }
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
		double sum = 0.0;
		while ((p = nexti++) != 0)
			sum += (double)((*p)%0);
		output%0 << sum;
	}
}
