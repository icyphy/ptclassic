defstar {
	name { Through }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Generates conditional code, depending on the value of
the Boolean state "control".
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { CG demo library }
	explanation {
This star exists only for demoing the generic CG domain.
It outputs lines of comments, instead of code.
	}
	state {
		name {control}
		type {int}
		default {FALSE}
		descriptor { Controls which of two code lines gets generated. }
	}
	input {
		name {input}
		type {FLOAT}
	}
	output {
		name {output}
		type {FLOAT}
	}
	codeblock (trueblock) {
// Line 1 from CGThrough if control == TRUE
// Line 2 from CGThrough if control == TRUE
	}
	codeblock (falseblock) {
// Line 1 from CGThrough if control == FALSE
// Line 2 from CGThrough if control == FALSE
	}
	go {
	    if (control == TRUE)
		addCode(trueblock);
	    else
		addCode(falseblock);
	}
}

