defstar {
	name { TestMultirate }
	domain { SDF }
	desc {
This star consumes and produces any fixed number of tokens on any number
of inputs and outputs.  The output value is always zero.  IT IS INTENDED
FOR TESTING SCHEDULERS ONLY.
	}
	version { @(#)SDFTestMultirate.pl	1.7  09/10/99 }
	author { Edward A. Lee, Richard Stevens, and Christopher Scannell }
	copyright {
Copyright (c) 1990-1999 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.

Richard Stevens and Christopher Scannell are employees of the
U.S. Government whose contributions to this computer program fall
within the scope of 17 U.S.C. A7 105.
	}
	location { SDF main library }
	inmulti {
		name { input }
		type { float }
	}
	outmulti {
		name { output }
		type { float }
	}
	state {
		name { consume }
		type { intarray }
		default { 1 }
		descriptor { number of inputs consumed for each port }
	}
	state {
		name { produce }
		type { intarray }
		default { 1 }
		descriptor { number of tokens produced on each output }
	}
	hinclude { <stdio.h> }
	setup {
		if ((input.numberPorts() != consume.size()) ||
		    (output.numberPorts() != produce.size())) {
		    Error::abortRun(*this,
			"the list of number of tokens consumed or produced ",
			"does not match the actual number of inputs or ",
			"outputs.");
		    return;
		}
		MPHIter nexti(input);
		DFPortHole* iportp;
		int i = 0;
		while ((iportp = (DFPortHole*)nexti++) != 0) {
		    iportp->setSDFParams(consume[i], 1);
		    i++;
		}
		MPHIter nexto(output);
		DFPortHole* oportp;
		i = 0;
		while ((oportp = (DFPortHole*)nexto++) != 0) {
		    oportp->setSDFParams(produce[i], 1);
		    i++;
		}
	}
	go {
		// FIXME: Why is there a printf statement in a go method?
		printf("Running %s\n", name());

		// Don't need to read each input port: SDF scheduler handles it
		// because we set setSDFParams for input ports in setup method

		// Output the right number of zeroes on each output port
		MPHIter nexto(output);
		DFPortHole* oportp;
		int j = 0;
		while ((oportp = (DFPortHole*)nexto++) != 0) {
			for (int i = 0; i < produce[j]; i++) {
				(*oportp)%i << 0.0;
			}
			j++;
		}
	}
}
