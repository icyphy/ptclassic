defstar {
	name { BlackHole }
	domain { AnyAsm }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$}
	author { J. Buck }
	desc {
Black hole star for assembly-code domains.  No code is generated at runtime.
	}
	input {
		name {input}
		type {anytype}
		attributes {P_CIRC}
	}
	start {
		int n = input.far()->numXfer();
		input.setSDFParams(n,n-1);
	}
	exectime {
		return 0;
	}
}
