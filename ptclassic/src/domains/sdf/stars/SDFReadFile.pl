defstar
{
    name { ReadFile }
    domain { SDF }
    version { $Id$ }
    author { T. M. Parks }
    copyright { 1991 The Regents of the University of California }
    location { SDF main library }
    descriptor { Read ASCII data from a file. }

    hinclude { <stream.h> }
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
	int fd = open(expandPathName(fileName), 0);
	if (fd < 0) {
		Error::abortRun(*this, "can't open file ", fileName);
		return;
	}
	LOG_NEW; input = new istream(fd);
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
		LOG_NEW; input = new istream(fd);
		(*input) >> x;		// get next value
		eatwhite(*input);
	    }
	}
	else			// get next value
	{
	    (*input) >> x;
	    eatwhite(*input);
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
