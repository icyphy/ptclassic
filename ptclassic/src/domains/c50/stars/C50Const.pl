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
	location { C50 main library }
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
		default {"0.0"}
		descriptor {Constant value to be output}
	}

	protected {
	    StringList constValue;
	}

	codeblock (org) {
	.ds	$addr(output)
	}
	codeblock (orgp) {
	.text
	}
	constructor {
		noInternalState();
	}
	initCode {
		constValue.initialize();
		double temp = level.asDouble();
		char	buf[32];
		sprintf(buf,"%.15f",temp);
		constValue <<"\t.q15\t"<< buf <<"\n";
		addCode(org);
		for (int i = 0; i < output.bufSize(); i++) addCode(constValue);
		addCode(orgp);
	}
	execTime {
		return 0;
	}
}
