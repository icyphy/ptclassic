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
		name { readClassName }
		access { public }
		type { "const char *" }
		code { return "Case"; }
	}
	go {
		// get Particles from Geodesic
		control.grabData();
		input.grabData();

		// read control value, and route input
		// to output depending on it.
		MPHIter nexti(output);
		PortHole* p;
		for (int i = int(control%0); i >= 0; i--)
			p = nexti++;

		(*p)%0 = input%0;
		p->sendData();
	}
}


