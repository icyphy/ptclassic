defstar {
	name { XYgraph }
	derivedFrom { Xgraph }
	domain { VHDL }
	version { $Id$ }
	desc {
Graph the X-Y input data.
	}
	author { Michael C. Williamson }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { VHDL main library }
	explanation {
	}
	input {
		name{ xInput }
		type{ ANYTYPE }
	}
	constructor {
		noInternalState();
	}
	codeblock (std) {
write($starSymbol(aline), $ref(xInput));
write($starSymbol(aline), $starSymbol(space));
write($starSymbol(aline), $ref(input));
writeline($starSymbol(fileout), $starSymbol(aline));
	}
	go {
	  addCode(std);
	}
}
