defcore {
    name { Printer }
    domain { ACS }
    coreCategory { FixSim }
    corona { Printer } 
    desc { Print out one sample from each input port per line.  The "fileName"
state specifies the file to be written; the special names
&lt;stdout&gt; and &lt;cout&gt;, which specify the standard output
stream, and &lt;stderr&gt; and &lt;cerr&gt;, which specify the
standard error stream, are also supported. }
    version { $Id$ }
    author { James Lundblad, Eric Pauer }
    copyright {
Copyright (c) 1998 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }

	defstate {
		name { fileNameFix }
		type { string }
		default { "<stdout>" }
		desc { Filename for output. }
	}
	hinclude { "pt_fstream.h" }
	protected {
		pt_ofstream *p_out;
	}
	constructor {
		p_out = 0;
	}
	setup {
		// in case file was open from previous run w/o wrapup call
		LOG_DEL; delete p_out;
		LOG_NEW; p_out = new pt_ofstream(fileNameFix);
	}
	go {
		pt_ofstream& output = *p_out;
		MPHIter nexti(corona.input);
		PortHole* p;
		while ((p = nexti++) != 0)
		 	output << ((*p)%0).print() << "\t";
		output << "\n";
	}
	wrapup {
		LOG_DEL; delete p_out;		// flush output
		p_out = 0;
	}
	destructor {
		LOG_DEL; delete p_out;		// flush output
	}
}

