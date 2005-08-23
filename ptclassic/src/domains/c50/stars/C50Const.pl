defstar {
	name { Const }
	domain { C50 }
	desc { Constant source }
	version { $Id$ }
	author { A. Baensch }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { C50 signal sources library }
	explanation {
There are no runtime instructions associated with this star.  The
output buffer is initialized with the specified DC value.
	}
	output {
		name {output}
		type {FIX}
		attributes{P_NOINIT}
	}
	state {
		name {level}
		type {FIX}
		default {0.001}
		descriptor { Constant value to go to the output. }
	}
	codeblock (org) {
	.ds	$addr(output)
	}
	codeblock (dc) {
	.q15	$val(level)
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
