defstar {
	name {Modulo}
	domain { CGC }
	desc {
The output of this star is the input modulo the "modulo" parameter.
The input and output are both float
	}
	version { @(#)CGCModulo.pl	1.3	7/9/96  }
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
	setup {
		if ( double(modulo) == 0.0 ) {
		    Error::abortRun(*this,
				    "The modulo parameter cannot be zero");
		    return;
		}
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
		return 1;
	}
}
