defstar {
	name {CxToRect}
	domain {CG56}
	desc { Convert complex data to real and imaginary parts. }
	version {$Id$}
	author { Jose Luis Pino }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
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
	move 	L:$addr(input),x
	move 	x1,$ref(real)
	move 	x0,$ref(imag)
	}
	go {
		addCode(convert);
	}
}
