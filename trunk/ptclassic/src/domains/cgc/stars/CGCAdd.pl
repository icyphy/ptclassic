defstar {
	name {Add}
	domain {CGC}
	desc { Output the sum of the inputs, as a floating value.  }
	version { $Id$ }
	author { S. Ha }
	copyright { 1991 The Regents of the University of California }
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
		StringList out;
		out << "\t$ref(output) = ";
		for (int i = 1; i <= input.numberPorts(); i++) {
			ix = i;
			out << "$ref(input#ix)";
			if (i < input.numberPorts()) out << " + ";
			else out << ";\n";
			addCode((const char*)out);
			out.initialize();
		}
	}
}
