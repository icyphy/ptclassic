defstar
{
    name { ReadFile }
    domain { SDF }
    version { $Id$ }
    author { T. M. Parks }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { SDF main library }
    descriptor { Read ASCII data from a file. }

    hinclude { "streamCompat.h" }
    ccinclude { "Scheduler.h" }

    output
    {
	name { output }
	type { float }
    }

    defstate
    {
	name { fileName }
	type { string }
	default { "/dev/null" }
	descriptor { Input file. }
    }

    defstate
    {
	name { haltAtEnd }
	type { int }
	default { "NO" }
	descriptor { Halt the run at the end of the input file. }
    }

    defstate
    {
	name { periodic }
	type { int }
	default { "YES" }
	descriptor { Make output periodic or zero-padded. }
    }

    protected
    {
	istream* input;
    }

    constructor { input = 0;   }
    destructor  { delete input;}

    start
    {
	LOG_DEL; delete input; input = 0;
	// open input file
	int fd = open(expandPathName(fileName), O_RDONLY);
	if (fd < 0) {
		Error::abortRun(*this, "can't open file ", fileName);
		return;
	}
	LOG_NEW; input = new ifstream(fd);
    }

    go
    {
	double x = 0.0;

	if (input->eof())
	{
	    if (haltAtEnd)		// halt the run
		Scheduler::requestHalt();
	    else if (periodic)		// close and re-open file
	    {
		LOG_DEL; delete input; input = 0;
		int fd = open(expandPathName(fileName), 0);
		if (fd < 0) {
			Error::abortRun(*this, "can't re-open file ", fileName);
			return;
		}
		LOG_NEW; input = new ifstream(fd);
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

    wrapup
    {
	LOG_DEL; delete input;
	input = 0;
    }
    destructor
    {
	LOG_DEL; delete input;
    }
}
