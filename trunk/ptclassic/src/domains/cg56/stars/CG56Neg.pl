defstar {
	name { Neg  }
	domain { CG56 }
	desc { Negates input }
	version { $Id$ }
	author { Chih-Tsung, ported from Gabriel }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 arithmetic library }
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
		addCode(negblock);
	}
}
