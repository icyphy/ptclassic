defstar {
	name { DeMux }
	domain { SDF }
	desc {
Demultiplexes one input onto any number of output streams.
The particle consumed on the input is copied to exactly one output,
determined by the "control" input.  The other outputs get a floating
point 0.0.  Integers from 0 through N-1 are accepted at the "control"
input, where N is the number of outputs.  If the control input is
outside this range, all outputs get 0.0.
	}
	version {$Id$}
	author { E. A. Lee }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	input {
		name {input}
		type {float}
	}
	input {
		name {control}
		type {int}
	}
	outmulti {
		name {output}
		type {float}
	}
	go {
	    int n = int(control%0);
	    MPHIter nexto(output);
	    for (int i = 0; i < output.numberPorts(); i++)
		if (i == n)
		    (*nexto++)%0 = input%0;
		else
		    (*nexto++)%0 << 0.0;
	}
}
