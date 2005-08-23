defstar {
	name { Sub }
	domain { SDF }
	desc { Output the "pos" input minus all "neg" inputs. }
	version {@(#)SDFSub.pl	2.9 3/2/95}
	author { E. A. Lee }
	copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
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
		double diff;
		diff = double(pos%0);
		MPHIter nexti(neg);
		for(int i=neg.numberPorts(); i>0; i--)
			diff -= double((*nexti++)%0);
		output%0 << diff;
	}
}


