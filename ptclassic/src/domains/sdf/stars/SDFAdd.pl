defstar {
	name {FloatSum}
	domain {SDF}
	desc { Output the sum of the inputs, as a floating value.  }
	version {$Id$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
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
			sum += double((*p)%0);
		output%0 << sum;
	}
}
