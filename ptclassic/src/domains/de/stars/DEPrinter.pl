defstar {
	name { Printer }
	domain { DE }
	desc {
Prints out one sample from each input port per line
If "fileName" is not equal to "cout" (the default), it
specifies the filename to print to.
	}
	version { $Id$}
	author { Soonhoi Ha }
	copyright { 1991 The Regents of the University of California }
	location { DE main library }
	explanation {
The input may be a particle of any type.  The print() method
of the particle is used to generate the output.
	}
	inmulti {
		name { input }
		type { ANYTYPE }
	}
	defstate {
		name { fileName }
		type { string }
		default { "cout" }
		desc { Filename for output, or "cout" for stdout. }
	}
	hinclude { "UserOutput.h" }
	protected {
		UserOutput output;
	}
	start {
		output.fileName(fileName);
	}

	go {
		// detect which input has an event and print out the
		// value and arrivalTime.
		InDEMPHIter nextp(input);
		int j = input.numberPorts();
		for(int i=1; i <= j; i++) {
			InDEPort& p = *nextp++;
			if (p.dataNew) {
				output << "(port#" << i << ") " <<
				"at time " << arrivalTime << " : value > "
				<< (p.get()).print() << "\n";
			}
		}
	}
	wrapup {
		output.flush();
	}
}

