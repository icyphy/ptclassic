defstar {
	name { Sub }
	domain { CGC }
	desc { Output the "pos" input minus all "neg" inputs. }
	version {$Id$}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name { pos }
		type { float }
	}
	inmulti {
		name { neg }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { index }
		type { int }
		default { 1 }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	constructor {
		noInternalState();
	}
	go { 
	   addCode(init); 
	   for (int i = 1; i <= neg.numberPorts(); i++) {
		index = i;
		addCode("\t$ref(output) -= $ref(neg#index);\n");
	   }
	}
	codeblock (init) {
	$ref(output) = $ref(pos);
	}
}
