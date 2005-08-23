defstar {
	name {ModuloInt}
	domain { CGC }
	desc { The output of this star is the input modulo the 
		"modulo" parameter. The input, output, and modulo
		are all integers. }
	version { @(#)CGCModuloInt.pl	1.2	2/13/96  }
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
        setup {
		if ( int(modulo) == 0 ) {
		    Error::abortRun(*this,
				    "The modulo parameter cannot be zero");
		    return;
		}
	}
	go {
		addCode(out);
	}
	codeblock(out) {
	$ref(output) = $ref(input) % $ref(modulo);
	}
	exectime {
		return 1;
	}
}
