defstar {
	name {Mpy}
	domain {CGC}
	desc { Output the product of the inputs, as a float value. }
	version { $Id$ }
	author { S. Ha }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CGC main library }
	inmulti {
		name {input}
		type {float}
	}
	output {
		name {output}
		type {float}
	}
	constructor {
		noInternalState();
	}
	go {
		StringList out;
		out << "\t$ref(output) = ";
		for (int i = 1; i <= input.numberPorts(); i++) {
			out << "$ref(input#" << i << ")";
			if (i < input.numberPorts()) out << " * ";
			else out << ";\n";
		}
		addCode((const char*)out);
	}
	exectime {
		return input.numberPorts();
	}
}
