defstar {
	name {CxToRect}
	domain {CGC}
	desc { Convert complex data to real and imaginary parts. }
	version { $Id$ }
	author { S. Ha }
	copyright { 1991 The Regents of the University of California }
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
