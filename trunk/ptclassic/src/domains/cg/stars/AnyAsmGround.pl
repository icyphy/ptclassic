defstar {
	name { Ground }
	domain { AnyAsm }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	version { $Id$ }
	author { Jose L. Pino }
	desc {
A source star for assembly-code domains.  No code is generated at runtime.
	}
	output {
		name {output}
		type {ANYTYPE}
		attributes {P_CIRC}
	}
	setup {
		int n = output.far()->numXfer();
		output.setSDFParams(n,n-1);
	}
	exectime {
		return 0;
	}
}
