defstar {
	name { Const }
	domain { CG56 }
	desc { Constant source }
	version { $Id$ }
	author { E. A. Lee,  J. Pino, J. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 demo library }
	explanation {
There are no runtime instructions associated with this star.  The
output buffer is initialized with the specified DC value.
	}
	output {
		name {output}
		type {FIX}
	}
	state {
		name {level}
		type {FIX}
		default {0.001}
		descriptor { Constant value to go to the output. }
	}
	codeblock (org) {
	org	$ref(output)
	}
	codeblock (dc) {
	dc	$val(level)
	}
	codeblock (orgp) {
	org	p:
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
