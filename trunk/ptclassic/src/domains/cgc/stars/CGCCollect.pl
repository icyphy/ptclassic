defstar {
	name { Collect }
	domain { CGC }
	desc {
Takes multiple inputs and produces one output.
	}
	version {$Id$ }
	author { S. Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
This star does not generate codes. In multiprocessor code generation domain,
it will be automatically attached to a porthole if the sources of the porthole
is more than one. Its role is just opposite to that of Spread star.
	}
	inmulti {
		name {input}
		type {ANYTYPE}
	}
	output {
		name {output}
		type {=input}
	}
	method {
		name { amISpreadCollect }
		access { protected }
		arglist { "()" }
		type { int }
		code {
			return 1;
		}
	}
	setup {
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

