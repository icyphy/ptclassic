defstar
{
    name { ReadFile }
    domain { SDF }
    version { @(#)SDFReadFile.pl	1.8	7/28/93 }
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
    ccinclude { "SimControl.h" }

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

    setup
    {
	// open input file
	LOG_NEW; input = new ifstream(expandPathName(fileName));
	if (!(*input))
		Error::abortRun(*this, "can't open file ", fileName);
    }

    go
    {
	double x = 0.0;

	if (input->eof())
	{
	    if (haltAtEnd)		// halt the run
		SimControl::requestHalt();
	    else if (periodic)		// close and re-open file
	    {
		LOG_DEL; delete input;
                LOG_NEW; input = new ifstream(expandPathName(fileName));
         	if (!(*input)) {
		    Error::abortRun(*this, "can't re-open file ", fileName);
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

    wrapup
    {
	LOG_DEL; delete input;
    }
}
