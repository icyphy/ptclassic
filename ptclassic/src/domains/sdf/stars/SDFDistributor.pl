defstar {
	name { Distributor }
	domain { SDF }
	version {$Id$}
	desc {
Takes one input stream and synchronously splits it into n output streams.
	}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
This star consumes enough inputs to produce one sample on each output.
	}
	input {
		name { input }
		type { ANYTYPE }
	}
	outmulti {
		name { output }
		type { =input }
	}
	start {
		int n = output.numberPorts();
		input.setSDFParams(n,n-1);
	}
// note: delay 0 is the newest sample, so we must read backwards
	go {
		MPHIter nexto(output);
		for (int i = output.numberPorts() - 1; i >= 0; i--)
			(*nexto++)%0 = input%i;
	}
}

