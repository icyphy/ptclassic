defstar {
	name { Printer }
	domain { DE }
	desc {
Print the value of each arriving event, together with its time of
arrival.  The "fileName" parameter specifies the file to be written;
the special names &lt;stdout&gt; and &lt;out&gt; (specifying the
standard output stream), and &lt;stderr&gt; and &lt;cerr&gt;
specifying the standard error stream, are also supported.
	}
	version { @(#)DEPrinter.pl	2.17	10/07/96}
	author { Soonhoi Ha and J. Buck}
	copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }
	htmldoc {
The input may be a particle of any type.
The print() method of the particle is used to generate the output.
If output is directed to a file, flushing does not occur until the
wrapup method is called.
Before the first data are flushed, the file will not even exist.
This is normal.
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
		pt_ofstream* p_out;
	}
	constructor {
		p_out = 0;
	}
	destructor {
		LOG_DEL; delete p_out;
	}
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
		int numports = input.numberPorts();
		for(int port = 1; port <= numports; port++) {
			InDEPort& iport = *nextp++;
			if (iport.dataNew) {
				output << "(port#" << port << ") "
				       << "at time " << arrivalTime
				       << " : value > "
				       << (iport.get()).print() << "\n";
			}
		}
	}
}
