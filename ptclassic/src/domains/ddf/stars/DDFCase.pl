defstar {
	name { Case }
	domain { DDF }
	desc {
Route an input particle to one of the outputs depending on the 
control particle.  The control particle should have value between
zero and N-1, inclusive, where N is the number of outputs.
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

	input {
		name { input }
		type { ANYTYPE }
	}
	input {
		name { control }
		type { int }
	}
	outmulti {
		name { output }
		type { =input }
		num { 0 }
	}
	go {
		// get Particles from Geodesic
		control.receiveData();
		input.receiveData();

		// read control value, and route input
		// to output depending on it.
		MPHIter nexti(output);
		PortHole* p = 0;
		for (int i = int(control%0); i >= 0; i--)
			p = nexti++;
		if (!p) {
			Error::abortRun (*this, "control value out of range");
			return;
		}
		(*p)%0 = input%0;
		p->sendData();
	}
}


