ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 8/17/90
 Converted to use preprocessor, 10/3/90

 Distributor takes one input stream and splits it up into n output
 streams (n may be any number).  It consumes n particles and produces
 1 particle on each of the n outputs.

**************************************************************************/
}

defstar {
	name { Distributor }
	domain { SDF }
	desc {}
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

