defstar {
	name { Source }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Generic code generator source star.
	}
	version {$Revision$ $Date$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG main library }
	output {
		name {output}
		type {FLOAT}
	}
	defstate {
		name {n}
		type {int}
		desc {number of samples to write}
		default {1}
	}
	setup {
		output.setSDFParams(n,int(n)-1);
	}
	codeblock (blockname) {
// Source star: write $val(n) samples
// Line 2 from Source star
	}
	go {
		addCode(blockname);
	}
}
