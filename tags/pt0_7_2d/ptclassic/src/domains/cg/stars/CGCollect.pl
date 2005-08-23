defstar {
	name { Collect }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Takes multiple inputs and produces one output.
	}
	version {@(#)CGCollect.pl	1.9	01 Oct 1996 }
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG demo library }
	htmldoc {
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
	setup {
		MPHIter iter(input);
		CGPortHole* p;
		int loc = 0;
		while ((p = (CGPortHole*) iter++) != 0) {
			output.embed(*p, loc);
			loc += p->numXfer();
		}
	}
	go {
	}
}

