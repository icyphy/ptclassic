defstar {
	name { BlackHole }
	domain { VHDL }
	version { @(#)VHDLBlackHole.pl	1.3 10/01/96 }
	desc {
Discards all inputs.
	}
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	htmldoc {
A BlackHole accepts input Particles, but doesn't do anything with
them.  It is typically used to discard unwanted outputs from other blocks.
	}
	input {
		name{ input }
		type{ ANYTYPE }
	}
	constructor {
		noInternalState();
	}
	codeblock (std) {
-- This star generates no code.  $ref(input) is unused here.
	}
	go {
	  addCode(std);
	}
	exectime {
		return 0;
	}
}
