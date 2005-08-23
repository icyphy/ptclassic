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
    version{ @(#)ACSPrinterFixSim.pl	1.13 08/02/01 }
    author { James Lundblad, Eric Pauer }
    copyright {
Copyright (c) 1998-2001 The Regents of the University of California
and Sanders, a Lockheed Martin Company
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { ACS main library }
	defstate {
	    name { LockInput }
	    type {int}
	    default {"NO"}
	    desc { 
Flag that indicates that the specified input precision should be used 
rather than modified by wordlength analysis in the FPGA domain }
	}

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

