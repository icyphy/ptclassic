defstar
{
    name { ReadFile }
    domain { SDF }
    version { $Id$ }
    author { T. M. Parks }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { SDF main library }
    descriptor {
Read ASCII data from a file.  The simulation can be halted
on end-of-file, or the file contents can be periodically
repeated, or the file contents can be padded with zeros.
	}
    hinclude { "streamCompat.h" }
    ccinclude { "SimControl.h" }

    output {
	name { output }
	type { float }
    }

    defstate {
	name { fileName }
	type { string }
	default { "/dev/null" }
	descriptor { Input file. }
    }

    defstate {
	name { haltAtEnd }
	type { int }
	default { "NO" }
	descriptor { Halt the run at the end of the input file. }
    }

    defstate {
	name { periodic }
	type { int }
	default { "YES" }
	descriptor { Make output periodic or zero-padded. }
    }

    protected {
	istream* input;
	char* expandedFileName;
    }

    constructor {
	input = 0;
	expandedFileName = 0;
    }

    setup {
	// open input file
	LOG_DEL; delete input;
	delete [] expandedFileName;
	expandedFileName = expandPathName(fileName);
	LOG_NEW; input = new ifstream(expandedFileName);
	if (!(*input))
		Error::abortRun(*this, "can't open file ", fileName);
    }

    go {
	double x = 0.0;

	if (input->eof())
	{
	    if (haltAtEnd)		// halt the run
		SimControl::requestHalt();
	    else if (periodic)		// close and re-open file
	    {
		LOG_DEL; delete input;
                LOG_NEW; input = new ifstream(expandedFileName);
         	if (!(*input)) {
		    Error::abortRun(*this, "cannot re-open file ", fileName);
		    return;
		}
		(*input) >> x;		// get next value
		ws(*input);
	    }
	}
	else			// get next value
	{
	    (*input) >> x;
	    ws(*input);
	}
	output%0 << x;
    }

    destructor {
	LOG_DEL; delete input;
	delete [] expandedFileName;
    }
}
