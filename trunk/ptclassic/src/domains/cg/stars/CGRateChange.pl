defstar {
	name { RateChange }
	domain { CG }
	derivedFrom { CGVarTime }
	desc {
Consumes "consume" samples and produces "produce" samples.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG demo library }
	explanation {
This star exists only for demoing the generic CG domain.
It outputs lines of comments, instead of code.
	}
	state {
		name {consume}
		type {int}
		default {1}
		descriptor { Controls how many samples are consumed. }
	}
	state {
		name {produce}
		type {int}
		default {1}
		descriptor { Controls how many samples are produced. }
	}
	input {
		name {input}
		type {FLOAT}
	}
	output {
		name {output}
		type {FLOAT}
	}
	setup {
		input.setSDFParams(int(consume),int(consume)-1);
		output.setSDFParams(int(produce),int(produce)-1);
	}
	codeblock (block) {
// RateChange star
	}
	go {
		addCode(block);
	}
}

