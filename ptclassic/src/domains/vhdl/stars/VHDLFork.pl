defstar {
	name { Fork }
	domain { VHDL }
	desc { Copy input to all outputs }
	version { @(#)VHDLFork.pl	1.4 10/01/96 }
	author { M. C. Williamson }
	copyright { 
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	explanation { Each input is copied to every output.  }
	input {
		name { input }
		type { ANYTYPE }
	}
	outmulti {
		name { output }
		type { = input }
	}
	setup {
		forkInit(input,output);
	}
        constructor {
	        noInternalState();
                output.setForkBuf(input);
//		setForkId();
        }
}
