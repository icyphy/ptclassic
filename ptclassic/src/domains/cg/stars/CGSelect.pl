defstar {
	name { Select }
	domain { CG }
	derivedFrom { CGVarTime }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	author { J. T. Buck }
	location { CG demo library }
	desc {
This star requires a BDF scheduler!

If the value on the 'control' line is nonzero, trueInput
is copied to the output; otherwise, falseInput is.
	}
	input {
		name { trueInput }
		type { ANYTYPE }
	}
	input {
		name { falseInput }
		type { =trueInput }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { =trueInput }
	}
	constructor {
		trueInput.setRelation(DF_TRUE,&control);
		falseInput.setRelation(DF_FALSE,&control);
	}
	codeblock (foo) {
// Select star
	}
	go {
		addCode(foo);
	}
}



