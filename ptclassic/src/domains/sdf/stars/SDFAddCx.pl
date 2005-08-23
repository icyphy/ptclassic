defstar {
	name {AddCx}
	domain {SDF}
	desc { Output the sum of the complex inputs as a complex value. }
	version {@(#)SDFAddCx.pl	2.12 09/01/97}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
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
		Complex sum = 0.0;
		// We use a temporary variable to avoid gcc2.7.2/2.8 problems
		Complex tmpsum;
		while ((p = nexti++) != 0) {
			tmpsum = (*p)%0;
			sum += tmpsum;
		}
		output%0 << sum;
	}
}
