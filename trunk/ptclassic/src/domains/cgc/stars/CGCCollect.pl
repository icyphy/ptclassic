defstar {
	name { Collect }
	domain { CGC }
	desc {
Takes multiple inputs and produces one output.
	}
	version {$Id$ }
	author { S. Ha }
	copyright { 1991 The Regents of the University of California }
	location { CG demo library }
	explanation {
This star does not generate codes. In multiprocessor code generation domain,
it will be automatically attached to a porthole if the sources of the porthole
is more than one. Its role is just opposite to that of Scatter star.
	}
	inmulti {
		name {input}
		type {ANYTYPE}
	}
	output {
		name {output}
		type {=input}
	}
	start {
		MPHIter iter(input);
		CGCPortHole* p;
		int loc = 0;
		while ((p = (CGCPortHole*) iter++) != 0) {
			output.embed(*p, loc);
			loc += p->numXfer();
		}
	}
	go {
	}
}

