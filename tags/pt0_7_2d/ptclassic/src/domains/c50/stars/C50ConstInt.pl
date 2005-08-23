defstar {
	name { ConstInt }
	domain { C50 }
	desc { Constant source }
	version {@(#)C50ConstInt.pl	1.8	05/26/98}
	author { Luis Gutierrez, Brian L. Evans, Edward A. Lee, Jose Luis Pino, and Joseph T. Buck, G. Arslan }
	copyright {
Copyright (c) 1990-1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	htmldoc {
There are no runtime instructions associated with this star.
The output buffer is initialized with the specified DC value.
	}
	output {
		name {output}
		type {int}
		
	}
	state {
		name {level}
		type {int}
		default {"0"}
		descriptor { Constant value to go to the output. }
		attributes { A_UMEM | A_SETTABLE | A_CONSTANT }
	}
	constructor {
		noInternalState();
	}
	
	execTime {
		return 0;
	}
}
