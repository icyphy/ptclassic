defcore {
    name { Fork }
    domain { ACS }
    coreCategory { FPCGC }
    corona { Fork } 
    desc { Copy input to all outputs }
    version { @(#)ACSForkFPCGC.pl	1.4 09/08/99}
    author { James Lundblad }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	constructor {
		noInternalState();
	}
	setup {
		forkInit(corona.input,corona.output);
	}
	exectime {
		return 0;
	}
}
