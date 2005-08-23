defstar {
	name { BlackHole }
	domain { AnyAsm }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { @(#)AnyAsmBlackHole.pl	1.6	1/1/96}
	author { J. Buck }
	desc {
Black hole star for assembly-code domains.  No code is generated at runtime.
	}
	input {
		name {input}
		type {anytype}
		attributes {P_CIRC}
	}
	setup {
		int n = input.far()->numXfer();
		input.setSDFParams(n,n-1);
	}
	exectime {
		return 0;
	}
}
