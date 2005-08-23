defstar {
	name { MultiInOut }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Takes multiple inputs and produces multiple outputs.
	}
	version {@(#)CGMultiInOut.pl	2.7	01 Oct 1996}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG demo library }
	htmldoc {
This star exists only for demoing the generic CG domain.
It outputs lines of comments, instead of code.
	}
	inmulti {
		name {input}
		type {FLOAT}
	}
	outmulti {
		name {output}
		type {FLOAT}
	}
	codeblock (block) {
// Multi Input and Output star
	}
	go {
		addCode(block);
	}
}
