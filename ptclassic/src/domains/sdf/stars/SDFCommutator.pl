ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  J. T. Buck
 Date of creation: 8/17/90
 Converted to use preprocessor, 9/26/90

 Commutator takes n input streams and combines them to form one
 output stream.  On each firing it consumes one particle from each
 input stream and produces the n particles on the output stream.

**************************************************************************/
}
defstar {
	name {Commutator}
	domain {SDF}
	inmulti {
		name {input}
		type {ANYTYPE}
	}
	output {
		name {output}
		type {ANYTYPE}
	}	
	constructor {
		input.inheritTypeFrom(output);
	}
	start {
		int n = input.numberPorts();
		output.setSDFParams(n,n-1);
	}
	go {
		for (int i = input.numberPorts()-1; i >= 0; i--)
			output%i = input()%0;
	}
}

