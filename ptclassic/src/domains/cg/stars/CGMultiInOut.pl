defstar {
	name { MultiInOut }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Takes multiple inputs and produces multiple outputs.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { CG demo library }
	explanation {
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
