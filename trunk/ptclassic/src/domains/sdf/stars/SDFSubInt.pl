defstar {
	name { SubInt }
	domain { SDF }
	desc { Output the pos input minus all neg inputs. }
	version { $Id$ }
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	input {
		name { pos }
		type { int }
	}
	inmulti {
		name { neg }
		type { int }
	}
	output {
		name { output }
		type { int }
	}
	go {
		int diff;
		diff = int(pos%0);
		MPHIter nexti(neg);
		for(int i=neg.numberPorts(); i>0; i--)
			diff -= int((*nexti++)%0);
		output%0 << diff;
	}
}


