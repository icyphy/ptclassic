defstar {
	name { EndCase }
	domain { DDF }
	desc {
Depending on the "control" particle, consume a particle from one of
the data inputs and send it to the output.  The control particle
should have value between zero and N-1, inclusive, where N is the
number of data inputs.  It can be used with Case to implement an
if ... then construct.
	}
	version { @(#)DDFEndCase.pl	1.16	3/6/96 }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DDF library }
	seealso { Case, HOFIfElse, HOFIfElseGr }
	inmulti {
		name { input }
		type { ANYTYPE }
		num { 0 }
	}
	input {
		name { control }
		type { int }
	}
	output {
		name { output }
		type { ANYTYPE }
	}
	protected {
		int readyToGo;
	}
	constructor {
		input.inheritTypeFrom(output);
	}
	setup {
		waitFor(control);
		readyToGo = FALSE;
	}
	go {
		// get control Particles from Geodesic
		if (!readyToGo) control.receiveData();	

		// test value of control string
		int inputNum = int(control%0);
		if ( inputNum < 0 ) {
			Error::abortRun(*this, "control value is negative");
			return;
		}
		else if ( inputNum >= input.numberPorts() ) {
			Error::abortRun(*this, "control value is too large");
			return;
		}

		InDDFMPHIter nexti(input);
		for (int i = 0; i < inputNum; i++) {
			nexti++;
		}
		InDDFPort& iport = *(InDDFPort *)nexti++;
		if (iport.numTokens() >= iport.numXfer()) {
			iport.receiveData();
			output%0 = iport%0;
			output.sendData();
			waitFor(control);
			readyToGo = FALSE;
		}
		else {
			waitFor(iport);
			readyToGo = TRUE;
		}
	}
}
