defstar {
	name {CxToRect}
	domain {CGC}
	desc { Convert complex data to real and imaginary parts. }
	version { $Id$ }
	author { S. Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
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
}
