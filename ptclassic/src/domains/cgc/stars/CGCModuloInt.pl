defstar {
	name {ModuloInt}
	domain { CGC }
	desc { The output of this star is the input modulo the 
		"modulo" parameter. The input, output, and modulo
		are all integers. }
	version { $Id$  }
	author { Siamak Modjtahedi }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name{input}
		type{int}
	}
	output {
		name{output}
		type{int}
	}
	defstate {
		name{modulo}
		type{int}
		default{"1"}
		desc {The modulo parameter}
	}
	constructor {
		noInternalState();
	}
	initCode {
		addInclude("<math.h>");
	}
	go {
		addCode(out);
	}
	codeblock(out) {
	$ref(output) = $ref(input) % $ref(modulo);
	}
	exectime {
		return 23;	/* value taken from CG96Sin */
	}
}
