defstar {
	name { Sink }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Swallows an input sample
	}
	version {$Revision$ $Date$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { CG demo library }
	explanation {
This star exists only for demoing the generic CG domain.
It outputs lines of comments, instead of code.
	}
	input {
		name {input}
		type {FLOAT}
	}
	codeblock (blockname) {
// Line 1 code from CGSink
// Line 2 code from CGSink
	}
	go {
		addCode(blockname);
	}
}

