defstar {
	name {Commutator}
	domain {SDF}
	version {$Id$}
	desc {
Takes n input streams and combines them to form one output stream.
	}
	author { J. T. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
This star synchronously combines streams.
On each firing it consumes one particle from each
input and produces the n particles on the output stream,
ordered according to the inputs.
	}
	inmulti {
		name {input}
		type {ANYTYPE}
	}
	output {
		name {output}
		type {=input}
	}	
	start {
		int n = input.numberPorts();
		output.setSDFParams(n,n-1);
	}
	go {
		MPHIter nexti(input);
		for (int i = input.numberPorts()-1; i >= 0; i--)
			output%i = (*nexti++)%0;
	}
}

