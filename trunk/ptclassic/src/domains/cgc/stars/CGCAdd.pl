defstar {
	name {Add}
	domain {CGC}
	desc { Output the sum of the inputs, as a floating value.  }
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
	state {
		name {ix}
		type { int }
		default { 1 }
		desc { index for multiple input trace }
		attributes { A_NONSETTABLE|A_NONCONSTANT }
	}
	constructor {
		noInternalState();
	}
	go {
		StringList out = "\t$ref(output) = ";
		for (int i = 1; i <= input.numberPorts(); i++) {
			// This code relies on side effects, i.e.,
			// the current value of the state ix
			ix = i;
			out << "$ref(input#ix)";
			if (i < input.numberPorts()) out << " + ";
			else out << ";\n";
			addCode(out);
			// Reinitialize out (indent the code)
			out = "\t\t";
		}
	}
	exectime {
		return input.numberPorts();
	}
}
