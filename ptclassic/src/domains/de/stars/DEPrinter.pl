ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 11/30/90

 Printer prints out its input, which may be any supported type.
 There may be multiple inputs: all inputs are printed together on
 the same line (separated by tab).

**************************************************************************/
}
defstar {
	name { Printer }
	domain { DE }
	desc {	"Prints out one sample from each input port per line\n"
		"If 'fileName' is not equal to 'cout' (the default), it\n"
		"specifies the filename to print to.\n"
	}
	inmulti {
		name { input }
		type { ANYTYPE }
	}
	defstate {
		name { fileName }
		type { string }
		default { "cout" }
		desc { "filename for output" }
	}
	protected {
		UserOutput output;
	}
	start {
		output.fileName(fileName);
		input.reset();
	}

	go {
		// detect which input has an event and print out the
		// value and arrivalTime.
		input.reset();
		int j = input.numberPorts();
		for(int i=1; i <= j; i++) {
			InDEPort& p = (InDEPort&) input();
			if (p.dataNew) {
				output << "(port#" << i << ") " <<
				"at time " << arrivalTime << " : value > "
				<< (p.get()).print() << "\n";
			}
		}
	}
}

