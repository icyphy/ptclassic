defstar {
	name {CxToRect}
	domain {CG56}
	desc { Convert complex data to real and imaginary parts. }
	version {$Id$}
	author { Jose Luis Pino }
	copyright {
Copyright (c) 1993 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 main library }
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
	codeblock (convert){
	move L:$addr(input),a
	move a1,$ref(real)
	move a0,$ref(imag)
	}
	go {
		addCode(convert);
	}
}
