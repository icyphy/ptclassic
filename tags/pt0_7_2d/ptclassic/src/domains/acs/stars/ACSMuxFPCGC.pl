defcore {
	name { Mux }
	domain { ACS }
	coreCategory { FPCGC }
	corona { Mux }
	desc { Takes the absolute value of the input. }
	version { @(#)ACSMuxFPCGC.pl	1.2 09/08/99 }
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
		if ( int(corona.blockSize) < 1 ) {
		 	Error::abortRun(*this, "blockSize must be positive");
			return;
		}
		corona.output.setSDFParams(int(corona.blockSize),int(corona.blockSize)-1);
		corona.input.setSDFParams(int(corona.blockSize),int(corona.blockSize)-1);
	}
	codeblock(init) {
	int n = $ref(control);
	int j = $val(blockSize);
	}
	codeblock(switchStatement) {
	switch(n)
	}
	codeblock(copydata,"int i, int portnum") {
	    case @i:
		while (j--) {
			$ref(output,j) = $ref(input#@portnum,j);
		}
		break;
	}
	codeblock(badPortNum) {
	    default:
		fprintf(stderr, "invalid control input %d", n);
	}
	initCode {
		addInclude("<stdio.h>");
	}
	go {
		addCode(init);
		addCode(switchStatement);
		addCode("\t{\n");
		// control value i means port number i+1
		for (int i = 0; i < corona.input.numberPorts(); i++) {
			addCode(copydata(i,i+1));
		}
		addCode(badPortNum);
		addCode("\t}\n");
	}
	exectime {
		return int(corona.blockSize) + 3;  
	}
}
