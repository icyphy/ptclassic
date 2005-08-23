defstar {
	name { Spread }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Takes one input and produces multiple outputs.
	}
	version {@(#)CGSpread.pl	1.9	01 Oct 1996}
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG demo library }
	htmldoc {
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
	setup {
		MPHIter iter(output);
		CGPortHole* p;
		int loc = 0;
		while ((p = (CGPortHole*) iter++) != 0) {
			input.embed(*p, loc);
			loc += p->numXfer();
		}
	}
	go {
	}
}
