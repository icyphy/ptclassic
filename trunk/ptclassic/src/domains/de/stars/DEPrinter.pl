defstar {
	name { Printer }
	domain { DE }
	desc {
Prints out one sample from each input port per line
If "fileName" is not equal to "<stdout>" (the default), it
specifies the filename to write to.  <stderr> prints on the
standard error stream.
	}
	version { $Id$}
	author { Soonhoi Ha }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
		default { "<stdout>" }
		desc { Filename for output }
	}
	hinclude { "pt_fstream.h" }
	protected {
		pt_ofstream output;
	}
	start {
		// abortRun is called on open failure
		output.open(fileName);
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
		output.close();
	}
}

