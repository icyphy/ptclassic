defstar {
	name { Abs }
	domain { CG56 }
	desc { Absolute value }
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
		addCode(absblock);
	}
}
