defstar {
	name { Through }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Generates conditional code, depending on the value of
the Boolean state "control".
	}
	version {@(#)CGThrough.pl	1.7 10/01/96}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG demo library }
	htmldoc {
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

