defstar {
	name { Mux }
	domain { SDF }
	desc {
Multiplexes any number of inputs onto one output stream.
A particle is consumed on each input, but only one of these is copied
to the output.  The one copied is determined by the "control" input.
Integers from 0 through N-1 are accepted at the "control" input,
where N is the number of inputs.  If the control input is outside
this range, an error is signaled.
	}
	version {$Revision$ $Date$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	inmulti {
		name {input}
		type {ANYTYPE}
	}
	input {
		name {control}
		type {int}
	}
	output {
		name {output}
		type {=input}
	}
	go {
	    int n = int(control%0);
	    if (n < 0) {
		Error::abortRun (*this, "Negative control to Mux ");
		return;
	    }
	    MPHIter nexti(input);
	    for (int i = 0; i < input.numberPorts(); i++)
		if (i == n) {
		    output%0 = (*nexti++)%0;
		    return;
		} else
		    nexti++;
	    Error::abortRun (*this, "Invalid control input to Mux");
	}
}
