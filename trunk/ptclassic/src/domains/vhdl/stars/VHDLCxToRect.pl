defstar {
	name { CxToRect }
	domain { VHDL }
	desc { Convert complex data to real and imaginary parts. }
	version { @(#)VHDLCxToRect.pl	1.2 03/07/96 }
	author { Michael C. Williamson, S. Ha }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
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
	codeblock (std) {
$ref(real) $assign(real) $refCx(input,real);
$ref(imag) $assign(imag) $refCx(input,imag);
	}
	go {
		addCode(std);
	}
}
