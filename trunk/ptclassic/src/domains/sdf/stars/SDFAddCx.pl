defstar {
	name {ComplexSum}
	domain {SDF}
	desc {Output the sum of the inputs, as a complex value.  }
	version {$Revision$ $Date$}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
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
		while ((p = nexti++) != 0)
			sum += Complex((*p)%0);
		output%0 << sum;
	}
}
