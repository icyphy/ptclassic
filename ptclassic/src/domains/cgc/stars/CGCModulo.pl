defstar {
	name {Modulo}
	domain { CGC }
	desc { The output of this star is the input modulo the 
		"modulo" parameter. The input, output are both
		 float }
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
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	defstate {
		name{modulo}
		type{float}
		default{"1.0"}
		desc {The modulo parameter}
	}
	constructor {
		noInternalState();
	}
	initCode {
		addInclude("<math.h>");
	}
	go {
		addCode(decl);
		addCode(out);
	}
	codeblock(decl) {
	double dummy;
	}
	codeblock(out) {
	$ref(output) = modf((double)$ref(input)/(double)$ref(modulo), &dummy);
	}
	exectime {
		return 23;	/* value taken from CG96Sin */
	}
}
