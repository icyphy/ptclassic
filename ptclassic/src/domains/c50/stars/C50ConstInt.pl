defstar {
	name { ConstInt }
	domain { C50 }
	desc { Constant source }
	version { @(#)C50ConstInt.pl	1.1	2/8/96 }
	author { Luis Gutierrez, Brian L. Evans, Edward A. Lee, Jose Luis Pino, Joseph T. Buck }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 main library }
	explanation {
There are no runtime instructions associated with this star.
The output buffer is initialized with the specified DC value.
	}
	output {
		name {output}
		type {int}
		attributes{P_NOINIT}
	}
	state {
		name {level}
		type {int}
		default {"0"}
		descriptor { Constant value to go to the output. }
	}
	constructor {
		noInternalState();
	}
	codeblock (org) {
	.data
	}
	codeblock (dc) {
	.word	 $val(level)
	}
	codeblock (orgp) {
	.text
	}
	initCode {
		addCode(org);
		for (int i=0 ; i<output.bufSize() ; i++) addCode(dc);
		addCode(orgp);
	}
	execTime {
		return 0;
	}
}
