defstar {
	name { Sink }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Swallows an input sample
	}
	version {1.9 01 Oct 1996}
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
	input {
		name {input}
		type {FLOAT}
	}
	defstate {
		name {n}
		type {int}
		desc {number of samples to write}
		default {1}
	}
	setup {
		input.setSDFParams(n,int(n)-1);
	}
	codeblock (blockname) {
// CGSink star: read $val(n) samples.
// Line 2 code from CGSink
	}
	go {
		addCode(blockname);
	}
}

