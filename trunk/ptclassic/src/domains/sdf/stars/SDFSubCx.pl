defstar {
	name { SubCx }
	domain { SDF }
	desc { Output the "pos" input minus the "neg" input, a complex value. }
	version {$Id$}
	author { J. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name { pos }
		type { complex }
	}
	input {
		name { neg }
		type { complex }
	}
	output {
		name { output }
		type { complex }
	}
	go {
		output%0 << Complex(pos%0) - Complex(neg%0);
	}
}


