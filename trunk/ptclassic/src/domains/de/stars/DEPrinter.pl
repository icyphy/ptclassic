defstar {
	name { Printer }
	domain { DE }
	desc {
Prints out one sample from each input port per line.  The "fileName"
state specifies the file to be written; the special names <stdout>
and <cout> (specifying the standard output stream), and <stderr> and <cerr>
specifying the standard error stream, are also supported.
	}
	version { $Id$}
	author { Soonhoi Ha and J. Buck}
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
.pp
If output is directed to a file, flushing does not occur until the
wrapup method is called.  Before the first data are flushed, the file
will not even exist.  This is normal.
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
		pt_ofstream *p_out;
	}
	constructor { p_out = 0;}
	destructor { LOG_DEL; delete p_out;}
	setup {
		// in case file was open from previous run w/o wrapup call
		LOG_DEL; delete p_out;
		LOG_NEW; p_out = new pt_ofstream(fileName);
	}
	go {
		pt_ofstream& output = *p_out;
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
		LOG_DEL; delete p_out;
		p_out = 0;
	}
}

