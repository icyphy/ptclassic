defstar {
	name { Neg  }
	domain { CG56 }
	desc { Negates input }
	version { @(#)CG56Neg.pl	1.11 01 Oct 1996 }
	author { Chih-Tsung, ported from Gabriel }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
	htmldoc {
<a name="negation"></a>
The input is moved into accumulator "a", where the "neg" instruction is applied.
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
	codeblock (negblock) {
	move	$ref(input),a
	neg	a
	move	a,$ref(output)
	}
	go {
		addCode(negblock);
	}
}
