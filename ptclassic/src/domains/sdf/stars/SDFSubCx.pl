defstar {
	name { SubCx }
	domain { SDF }
	desc { Output the "pos" input minus the "neg" input, a complex value. }
	version {$Id$}
	author { J. Buck }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
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
		// We use temporary variables to avoid gcc2.7.2/2.8 problems
		Complex tmppos = (pos%0);
		Complex tmpneg = (neg%0);
		output%0 << tmppos - tmpneg;
	}
}


