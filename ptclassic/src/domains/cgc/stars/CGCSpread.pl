defstar {
	name { Spread }
	domain { CGC }
	desc {
Takes one input and produces multiple outputs.
	}
	version {$Id$}
	author { S. Ha }
	copyright { 1991 The Regents of the University of California }
	location { CGC main library }
	explanation {
This star does not generate any code. In multiprocessor code generation
domain, this star will be automatically attached to a porthole whose
outputs are passed to more than one destination (one ordinary block and
one Send star, more than one Send stars, and so on.)
	}
	input {
		name {input}
		type {ANYTYPE}
	}
	outmulti {
		name {output}
		type {=input}
	}
	start {
		MPHIter iter(output);
		CGCPortHole* p;
		int loc = 0;
		while ((p = (CGCPortHole*) iter++) != 0) {
			input.embed(*p, loc);
			loc += p->numXfer();
		}
	}
	go {
	}
}
