ident {
// Output = pos input minus all neg inputs
// Version identification:
// $Id$

// Copyright (c) 1990 The Regents of the University of California.
//                       All Rights Reserved.

// Programmer: E. A. Lee
// Date of creation: 9/19/90
// Converted to use preprocessor: 10/3/90 by JTB
}
defstar {
	name { FloatDiff }
	domain { SDF }
	desc { "Output the pos input minus all neg inputs" }
	input {
		name { pos }
		type { float }
	}
	inmulti {
		name { neg }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	go {
		float diff;
		diff = float(pos%0);
		MPHIter nexti(neg);
		for(int i=neg.numberPorts(); i>0; i--)
			diff -= float((*nexti++)%0);
		output%0 << diff;
	}
}


