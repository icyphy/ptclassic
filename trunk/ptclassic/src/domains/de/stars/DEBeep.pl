defstar {
	name { Beep }
	domain { DE }
	desc { Cause a beep on the terminal when fired. }
	version { $Id$ }
	author { T. M. Parks }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { DE main library }

	hinclude { "pt_fstream.h" }

	input
	{
	    name { trigger }
	    type { anyType }
	}

	protected
	{
	    pt_ofstream output;
	}

	start
	{
	    // abortRun is called on open failure
	    output.open("<cerr>");
	}

	go
	{
	    output << '\007';	// ASCII BEL character.
	}

	wrapup
	{
		output.flush();
	}
}
