defstar {
	name { Fork }
	domain { CG }
	desc { Fork for CG dummy domain }
	version { @(#)CGFork.pl	1.4 01/01/96 }
	author { J. Buck }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	input {
		name {input}
		type {anytype}
	}
	outmulti {
		name {output}
		type {=input}
	}
	setup {
		forkInit(input,output);
	}
}
