defstar {
	name { Neg  }
	domain { CG56 }
	desc { Negates input }
	version { $Id$ }
	author { Chih-Tsung, ported from Gabriel }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
.Id "negation"
The input is moved into accumlator "a", where the "neg" instruction is applied.
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
	codeblock (negblock) {
	move	$ref(input),a
	neg	a
	move	a,$ref(output)
	}
	go {
		gencode(negblock);
	}
}
