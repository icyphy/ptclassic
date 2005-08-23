defstar {
	name {BlackHole}
	domain {CGC}
	version {@(#)CGCBlackHole.pl	1.13	01 Oct 1996}
	desc {
Discards all inputs.
	}
	author { J. T. Buck }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	htmldoc {
A BlackHole accepts input Particles, but doesn't do anything with
them.  It is typically used to discard unwanted outputs from other blocks.
	}
	inmulti {
		name{input}
		type{ANYTYPE}
	}
	constructor {
		noInternalState();
	}
	go {
		addCode("/* This star generates no code */");
	}
	exectime {
		return 0;
	}
}
