defstar {
	name { EndCase }
	domain { DDF }
	desc {
Depending on the "control" input, route an "input" to
the "output".
	}
	version { $Id$ }
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
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
	method {
		name { readClassName }
		access { public }
		type { "const char *"}
		code { return "EndCase";}
	}

	constructor {
		input.inheritTypeFrom(output);
	}
	start {
		waitFor(control);
		readyToGo = FALSE;
	}
	go {
	// get control Particles from Geodesic
		if (!readyToGo)
			control.grabData();	
		
		MPHIter nexti(input);
		PortHole* p = 0;
		for (int i = int(control%0); i >= 0; i--)
			p = nexti++;
		if (!p) {
			Error::abortRun (*this, "control value out of range");
			return;
		}
		if (p->numTokens() >= p->numberTokens) {
			p->grabData();
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


