defstar {
	name { Fork }
	domain { CGC }
	desc { Copy input to all outputs }
	version { $Id$ }
	author { E. A. Lee }
	copyright { 1992 The Regents of the University of California }
	location { CGC demo library }
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
	ccinclude { "CGCGeodesic.h" }
	constructor {
		output.setForkBuf(input);
	}
	exectime {
		return 0;
	}
}
