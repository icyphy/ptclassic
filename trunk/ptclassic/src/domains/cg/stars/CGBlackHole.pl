defstar {
	name { BlackHole }
	domain { CG }
	author { Raza Ahmed, Joseph T. Buck, and Brian L. Evans }
	desc {
Black hole star for code generation domains.
No code is generated at runtime.
	}
	version { $Id$}
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	input {
		name {input}
		type {anytype}
	}
	setup {
		int n = input.far()->numXfer();
		input.setSDFParams(n,n-1);
	}
	exectime {
		return 0;
	}
}
