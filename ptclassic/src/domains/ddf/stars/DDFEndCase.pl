defstar {
	name { EndCase }
	domain { DDF }
	desc {
Depending on the "control" particle, consume a particle from one of
the data inputs and send it to the output.  The control particle
should have value between zero and N-1, inclusive, where N is the
number of data inputs.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DDF library }

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
		type { anytype }
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
		if (!readyToGo)
			control.receiveData();	
		
		InDDFMPHIter nexti(input);
		InDDFPort* p = 0;
		for (int i = int(control%0); i >= 0; i--)
			p = nexti++;
		if (!p) {
			Error::abortRun (*this, "control value out of range");
			return;
		}
		if (p->numTokens() >= p->numXfer()) {
			p->receiveData();
			output%0 = (*p)%0;
			output.sendData();
			waitFor(control);
			readyToGo = FALSE;
		} else {
			waitFor(*p);
			readyToGo = TRUE;
			return;
		}
	}
}


