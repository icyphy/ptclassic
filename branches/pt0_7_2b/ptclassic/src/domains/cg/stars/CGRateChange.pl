defstar {
	name { RateChange }
	domain { CG }
	desc {
Consumes "consume" samples and produces "produce" samples.
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
	codeblock (block) {
// RateChange star
	}
	go {
		gencode(block);
	}
}

