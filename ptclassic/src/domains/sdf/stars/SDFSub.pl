defstar {
	name { FloatDiff }
	domain { SDF }
	desc { Output the "pos" input minus all "neg" inputs. }
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
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


