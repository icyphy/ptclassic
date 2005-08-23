defstar {
	name { Abs }
	domain { CG56 }
	desc { Absolute value }
	version { @(#)CG56Abs.pl	1.13	01 Oct 1996 }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="absolute value"></a>
The input is moved into accumulator "a", where the "abs" instruction is applied.
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
	constructor {
		noInternalState();
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
