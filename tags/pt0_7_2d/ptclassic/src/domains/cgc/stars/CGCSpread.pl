defstar {
	name { Spread }
	domain { CGC }
	desc {
Takes one input and produces multiple outputs.
	}
	version {@(#)CGCSpread.pl	1.9	01 Oct 1996}
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
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
	method {
		name { amISpreadCollect }
		access { protected }
		arglist { "()" }
		type { int }
		code {
			return -1;
		}
	}
	setup {
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
