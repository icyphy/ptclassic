defstar {
	name { XCABase }
	domain { CG56 }
	desc { S56X to CGC asynchronous IPC base star }
	version { $Id$ }
	author { Jose Luis Pino }
	copyright {
Copyright (c) 1994,1993 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	hinclude  { "CGCS56XTarget.h"}
	location { CG56 Target Directory }
	explanation {
	}

	state {
		name {value}
		type {int}
		default {0}
		attributes { A_YMEM|A_NOINIT } 
		desc { The initial value of asynchronous I/O. }
	}

 	state {
 		name { VariableName }
 		type { string }
 		default { "aioVariable" }
		desc { The name used to identify the asynchronous connection. }
 	}

	codeblock(initValue) {
	org	$ref(value)
$val(VariableName)
	dc	$val(value)
	}

initCode {
	addCode(initValue);
}

}
