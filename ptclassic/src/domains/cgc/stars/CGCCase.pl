defstar {
	name { Case }
	domain { CGC }
	desc {
This star routes an "input" token to one "output" depending on the 
"control" token. But, no code is generated in this star since a ddf-target
will generate the code.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }

	input {
		name { input }
		type { ANYTYPE }
	}
	input {
		name { control }
		type { int }
	}
	outmulti {
		name { output }
		type { =input }
		num { 0 }
	}
	codeblock (block) {
/* Case star */
	}
	go {
		addCode(block);
	}
}


