defstar {
	name { Fork }
	domain { CGC }
	desc { Copy input to all outputs }
	version { @(#)CGCFork.pl	1.12	01 Oct 1996 }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC demo library }
	htmldoc {
Each input is copied to every output.  This is done by the way the buffers
are laid out; no code is required.
	}
	input {
		name {input}
		type {ANYTYPE}
	}
	outmulti {
		name {output}
		type {=input}
	}
	constructor {
		noInternalState();
	}
	setup {
		forkInit(input,output);
	}
	exectime {
		return 0;
	}
}
