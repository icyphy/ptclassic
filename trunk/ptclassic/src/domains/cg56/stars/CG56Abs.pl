defstar {
	name { Abs }
	domain { CG56 }
	desc { Absolute value }
	version { $Id$ }
	author { E. A. Lee }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
.Id "absolute value"
The input is moved into accumlator "a", where the "abs" instruction is applied.
	}
	execTime {
		return 3;
	}
	input {
		name {input}
		type {FIX}
	}
	output {
		name {output}
		type {FIX}
	}
	codeblock (absblock) {
	move	$ref(input),a
	abs	a
	move	a,$ref(output)
	}
	go {
		gencode(absblock);
	}
}
