defstar {
	name { Abs }
	domain { CG56 }
	desc { Absolute value }
	version { $Id$ }
	author { E. A. Lee }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
The input is moved into accumlator "a", where the "abs" instruction is applied.
	}
	execTime {
		return 3;
	}
	input {
		name {in}
		type {FLOAT}
	}
	output {
		name {out}
		type {FLOAT}
	}
	codeblock (absblock) {
	move	$loc(in),a
	abs	a
	move	a,$loc(out)
	}
	go {
		gencode(absblock);
	}
}
