defstar {
	name { Source }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Generic code generator source star.
	}
	version {$Revision$ $Date$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { CG main library }
	output {
		name {output}
		type {FLOAT}
	}
	codeblock (blockname) {
// Line 1 from Source star
// Line 2 from Source star
	}
	go {
		addCode(blockname);
	}
}
