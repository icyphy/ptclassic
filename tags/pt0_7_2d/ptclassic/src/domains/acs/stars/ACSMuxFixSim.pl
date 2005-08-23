defcore {
	name { Mux }
	domain { ACS }
	coreCategory { FixSim }
	corona { Mux }
	desc { Takes the absolute value of the input. }
	version { @(#)ACSMuxFixSim.pl	1.2 09/08/99 }
	author { James Lundblad }
	copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    	}
    	location { ACS main library }
	setup {
		if ( int(corona.blockSize) <= 0 ) {
			Error::abortRun(*this, "blockSize must be positive");
			return;
		}
		corona.output.setSDFParams(int(corona.blockSize),int(corona.blockSize)-1);
		corona.input.setSDFParams(int(corona.blockSize),int(corona.blockSize)-1);
	}
	go {
		int numports = corona.input.numberPorts();
		int n = int(corona.control%0);
		int j = int(corona.blockSize);
		if (n >= 0 && n < numports) {
			MPHIter nexti(corona.input);
			PortHole* p = 0;
			for (int i = 0; i <= n; i++) {
				p = nexti++;
			}
			while (j--) {
				corona.output%j = (*p)%j;
			}
		}
		else {
			StringList msg = "Control input ";
			msg << n << " is out of the range [0,"
			    << (numports - 1) << "]";
			Error::abortRun(*this, msg);
			return;
		}
	}
}

