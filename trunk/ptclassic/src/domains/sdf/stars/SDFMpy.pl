defstar {
	name {Mpy}
	domain {SDF}
	desc { Output the product of the inputs, as a float value. }
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
		double product = 1.0;
		while ((p = nexti++) != 0)
			product *= double((*p)%0);
		output%0 << product;
	}
}
