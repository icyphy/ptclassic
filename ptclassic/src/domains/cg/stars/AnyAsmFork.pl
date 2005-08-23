defstar {
	name { Fork }
	domain { AnyAsm }
	desc {
Fork star for assembly-code domains.  No code is generated at runtime.
	}
	version {@(#)AnyAsmFork.pl	1.8 01/01/96}
	author { J. Buck }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { included in all assembly domains }
	input {
		name {input}
		type {anytype}
		attributes { P_CIRC }
	}
	outmulti {
		name {output}
		type {=input}
	}
	exectime {
		return 0;
	}
	setup {
		forkInit(input,output);
	}
}
