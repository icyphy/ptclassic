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
 		name { VariableName }
 		type { string }
 		default { "aioVariable" }
		desc { The name used to identify the asynchronous connection. }
 	}
	
	state {
		name { blockSize }
		type { int }
		default { 1 }
		desc { number of tokens to be transfered per firing. }
	}

	state {
		name {buffer}
		type {intarray}
		default {0}
		attributes { A_YMEM|A_NOINIT|A_NONSETTABLE } 
	}

	state {
		name {semaphore}
		type {int}
		default {0}
		attributes {A_YMEM|A_NOINIT}
	}
		
codeblock(initBuffer) {
	org	$ref(semaphore)
$val(VariableName)_sem
	dc	0
	org	$ref(buffer)
$val(VariableName)
}

setup {
	if (blockSize > 1) {
		buffer.resize(blockSize);
	}
}

initCode {
	addCode(initBuffer);
	for (int i = 0 ; i < blockSize ; i++) {
		addCode("	dc	0\n");
	}
	addCode("	org	p:\n");
}

}
