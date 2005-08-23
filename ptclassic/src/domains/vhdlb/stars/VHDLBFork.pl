defstar {
	name { Fork }
	domain { VHDLB }
	desc { Copy input to all outputs }
	version { @(#)VHDLBFork.pl	1.6 01 Oct 1996 }
	author { M. C. Williamson }
	copyright { 
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDLB main library }
	explanation { Each input is copied to every output.  }
	input {
		name {input}
		type { int }
	}
	outmulti {
		name {output}
		type { int }
	}
        constructor {
                output.setForkBuf(input);
		setForkId();
        }
}
