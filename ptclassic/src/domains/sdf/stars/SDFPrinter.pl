defstar {
	name { Printer }
	domain { SDF }
	desc {
Prints out one sample from each input port per line.  The "fileName"
state specifies the file to be written; the special names <stdout>
and <cout> (specifying the standard output stream), and <stderr> and <cerr>
specifying the standard error stream, are also supported.
	}
	version {$Id$}
	author { D. G. Messerschmitt and J. Buck }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { SDF main library }
	explanation {
This star prints its input, which may be any supported type.
There may be multiple inputs: all inputs are printed together on
the same line, separated by tabs.
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
		desc { Filename for output. }
	}
	hinclude { "pt_fstream.h" }
	protected {
		pt_ofstream *p_out;
	}
	constructor { p_out = 0;}
	destructor { LOG_DEL; delete p_out;}
	wrapup {
		LOG_DEL; delete p_out;
		p_out = 0;
	}
	setup {
		// in case file was open from previous run w/o wrapup call
		LOG_DEL; delete p_out;
		LOG_NEW; p_out = new pt_ofstream(fileName);
	}
	go {
		pt_ofstream& output = *p_out;
		MPHIter nexti(input);
		PortHole* p;
		while ((p = nexti++) != 0)
		 	output << ((*p)%0).print() << "\t";
		output << "\n";
	}
}

