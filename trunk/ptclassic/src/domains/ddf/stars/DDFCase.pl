defstar {
	name { Case }
	domain { DDF }
	desc {
This star routes an "input" token to one "output" depending on the 
"control" token.
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
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
	method {
		name { readTypeName }
		access { public }
		type { "const char *" }
		code { return "Case"; }
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


