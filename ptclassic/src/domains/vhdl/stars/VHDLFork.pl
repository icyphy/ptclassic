defstar {
	name { Fork }
	domain { VHDL }
	desc { Copy input to all outputs }
	version { $Id$ }
	author { M. C. Williamson }
	copyright { 
Copyright (c) 1990-%Q% The Regents of the University of California.
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
        constructor {
                output.setForkBuf(input);
//		setForkId();
        }
}
