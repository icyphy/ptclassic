defstar {
	name { EndCase }
	domain { CGC }
	desc {
Depending on the "control" input, route an "input" to
the "output". No code is generated here since target will generate it instead.
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

	inmulti {
		name { input }
		type { ANYTYPE }
		num { 0 }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { =input }
	}
	codeblock (block) {
/* EndCase star */
	}
	go {
		addCode(block);
	}
}


