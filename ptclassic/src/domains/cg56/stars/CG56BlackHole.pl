defstar {
	name { BlackHole }
	domain { CG56 }
	desc { Discards the inputs }
	version { $Id$ }
	author { E. A. Lee }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
Does nothing.  Input is ignored.
	}
	execTime {
		return 0;
	}
	inmulti {
		name {input}
		type {ANYTYPE}
	}
}
