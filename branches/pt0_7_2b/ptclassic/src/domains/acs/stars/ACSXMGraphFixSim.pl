defcore {
	name { XMGraph }
	domain { ACS }
	coreCategory { FixSim }
	corona { XMGraph }
        version {@(#)ACSXMGraphFixSim.pl	1.4 09/08/99}
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
    defstate {
	    name { LockInput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified input precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}
    setup {
		corona.graph.initialize(this, corona.input.numberPorts(),
			corona.options, corona.title, corona.saveFile, corona.ignore);
		corona.index = corona.xInit;
    }

	go {
		MPHIter nexti(corona.input);
		for (int i = 1; i <= corona.input.numberPorts(); i++) {
			corona.graph.addPoint(i, corona.index, float((*nexti++)%0));
		}
		corona.index = double(corona.xUnits) + corona.index;
	}
	wrapup {
		corona.graph.terminate();
	}
}
