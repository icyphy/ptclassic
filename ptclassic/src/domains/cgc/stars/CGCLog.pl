defstar {
	name {Log}
	domain { CGC }
	desc { Outputs natural log of input. }
	version { @(#)CGCLog.pl	1.7	01 Oct 1996 }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC nonlinear library }
	htmldoc {
Outputs natural log of input.
If the input is zero or negative, the constant -100.0 is produced.
<a name="logarithm"></a>
	}
	input {
		name{input}
		type{float}
	}
	output {
		name{output}
		type{float}
	}
	constructor {
		noInternalState();
	}
	initCode {
		addInclude("<math.h>");
	}
	go {
		addCode(ln);
	}
   codeblock(ln) {
	if($ref(input) <= 0) $ref(output) = -100.0;
	else $ref(output) = log($ref(input));
   }
	exectime {
		return 33;  /* based on CG96Log */
	}
}
