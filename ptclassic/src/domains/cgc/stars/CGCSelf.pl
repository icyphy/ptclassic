defstar {
	name { Self }
	domain { CGC }
	desc { Realizes recursion. }
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	explanation {
This is a star for recursion.  At runtime, it appears to contain
a clone of the galaxy whose name is given by "recurGal".
That galaxy is supplied with the input particles and executed.
That galaxy may be one within which this instance of Self resides,
hence realizing recursion.
At compile time, this star appears to just be an atomic star.
	}
	seealso { fibonnacci }
	inmulti {
		name { input }
		type { float }
	}
	outmulti {
		name { output }
		type { float }
	}
	codeblock (block) {
/* Self star */
	}
	go {
		addCode(block);
	}
}

