defstar {
	name { FixToInt }
	domain { CGC }
	version { $Id$ }
	author { Y.K. Lim }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	desc { Convert a fixed-point input to an integer output. }
	input {
		name { input }
		type { fix }
		desc { Input fix type }
	}
	output {
		name { output }
		type { int }
		desc { Output int type }
	}
	codeblock(fixtoint) {
	        $ref(output) = FIX_Fix2Int($ref(input));
	}
        go {
                addCode(fixtoint);
	}
}
