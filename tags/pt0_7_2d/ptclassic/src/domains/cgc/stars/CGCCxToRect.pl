defstar {
	name {CxToRect}
	domain {CGC}
	desc { Convert complex data to real and imaginary parts. }
	version { @(#)CGCCxToRect.pl	1.6	1/1/96 }
	author { S. Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	input {
		name {input}
		type {complex}
	}
	output {
		name {real}
		type {float}
	}
	output {
		name {imag}
		type {float}
	}
	constructor {
		noInternalState();
	}
	codeblock(body) {
	$ref(real) = $ref(input).real;
	$ref(imag) = $ref(input).imag;
	}
	go {
		addCode(body);
	}
	exectime {
		return 2;
	}
}
