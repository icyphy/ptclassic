defstar {
	name {CxToRect}
	domain {C50}
	desc { Convert complex data to real and imaginary parts. }
	version { $Id$ }
	author { Luis Gutierrez }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	input {
		name {input}
		type {COMPLEX}
	}
	output {
		name {real}
		type {FIX}
	}
	output {
		name {imag}
		type {FIX}
	}
	constructor {
		noInternalState();
	}
	codeblock (convert){
	lmmr	ar0,#$addr(input,0)
	lmmr	ar1,#$addr(input,1)
	smmr	ar0,#$addr(real)
	smmr	ar1,#$addr(imag)
	}
	go {
		addCode(convert);
	}

	exectime{
		return 4;
	}
}
