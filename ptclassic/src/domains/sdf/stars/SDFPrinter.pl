defstar {
	name { Printer }
	domain { SDF }
	desc {
Prints out one sample from each input port per line
If "fileName" is not equal to "cout" (the default), it
specifies the filename to write to.
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
		default { "cout" }
		desc { Filename for output. }
	}
	hinclude { "UserOutput.h" }
	protected {
		UserOutput output;
	}
	wrapup {
		output.flush();
	}
	start {
		if (!output.fileName(fileName))
			Error::abortRun(*this,"can't open file ",fileName);
	}

	go {
		MPHIter nexti(input);
		PortHole* p;
		while ((p = nexti++) != 0)
		 	output << ((*p)%0).print() << "\t";
		output << "\n";
	}
}

