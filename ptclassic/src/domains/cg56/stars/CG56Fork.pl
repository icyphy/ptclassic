defstar {
	name { Fork }
	domain { CG56 }
	desc { Copy input to all outputs }
	version { $Id$ }
	author { J. Buck }
	copyright { 1992 The Regents of the University of California }
	location { CG56 demo library }
	explanation {
Each input is copied to every output.  This is done by the way the buffers
are laid out; no code is required.
	}
	input {
		name {input}
		type {ANYTYPE}
	}
	outmulti {
		name {output}
		type {=input}
	}
	ccinclude { "AsmGeodesic.h" }
	constructor {
		output.setForkBuf(input);
		isaFork();
	}
	exectime {
		return 0;
	}
}
