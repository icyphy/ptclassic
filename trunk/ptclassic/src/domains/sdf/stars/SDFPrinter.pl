defstar {
	name { Printer }
	domain { SDF }
	desc {
Prints out one sample from each input port per line
If "fileName" is not equal to "<stdout>" (the default), it
specifies the filename to write to.  <stderr> prints on the
standard error stream.
	}
	version {$Id$}
	author { D. G. Messerschmitt and J. Buck }
	copyright { 1991 The Regents of the University of California }
	location { SDF main library }
	explanation {
This star prints its input, which may be any supported type.
There may be multiple inputs: all inputs are printed together on
the same line, separated by tabs.
	}
	inmulti {
		name { input }
		type { ANYTYPE }
	}
	defstate {
		name { fileName }
		type { string }
		default { "<stdout>" }
		desc { Filename for output. }
	}
	hinclude { "pt_fstream.h" }
	protected {
		pt_ofstream output;
	}
	wrapup {
		output.flush();
	}
	start {
		// abortRun is called on open failure
		output.open(fileName);
	}

	go {
		MPHIter nexti(input);
		PortHole* p;
		while ((p = nexti++) != 0)
		 	output << ((*p)%0).print() << "\t";
		output << "\n";
	}
}

