defstar {
	name { Beep }
	domain { DE }
	desc { Cause a beep on the terminal when fired. }
	version { $Id$ }
	author { T. M. Parks }
	copyright { 1992 The Regents of the University of California }
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
