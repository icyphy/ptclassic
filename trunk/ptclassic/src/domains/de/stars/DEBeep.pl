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

	ccinclude { "iostream.h" }

	input
	{
	    name { trigger }
	    type { anyType }
	}

	go
	{
	    cerr << '\007';	// ASCII BEL character.
	}
}
