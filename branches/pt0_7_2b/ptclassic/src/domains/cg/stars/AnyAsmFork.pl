defstar {
	name { Fork }
	domain { AnyAsm }
	desc {
Fork star for assembly-code domains.  No code is generated at runtime.
	}
	version {$Id$}
	author { J. Buck }
	copyright { 1992 The Regents of the University of California }
	location { included in all assembly domains }
	input {
		name {input}
		type {anytype}
	}
	outmulti {
		name {output}
		type {=input}
	}
	constructor {
		output.setForkBuf(input);
		isaFork();
	}
	exectime {
		return 0;
	}
	// fork is never deferrable -- force those tokens through
	method {
		name { deferrable }
		type { int }
		code { return FALSE;}
	}
	start {
		int n = input.far()->numXfer();
		input.setSDFParams(n,n-1);
		output.setSDFParams(n,n-1);
		int srcDelay = input.numTokens();
		if (srcDelay > 0) {
			// move delays from input to outputs
			PortHole* f = input.far();
			input.disconnect();
			input.connect(*f,0);
			MPHIter nextp(output);
			PortHole* p;
			while ((p = nextp++) != 0) {
				int n = p->numTokens();
				f = p->far();
				p->disconnect();
				p->connect(*f,n+srcDelay);
			}
			// re-initialize all neighbors.
			input.far()->parent()->initialize();
			nextp.reset();
			while ((p = nextp++) != 0) {
				p->far()->parent()->initialize();
			}
		}
	}
}
