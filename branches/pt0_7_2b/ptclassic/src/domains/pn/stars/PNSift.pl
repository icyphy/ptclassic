defstar
{
    name { Sift }
    derivedFrom { Splice }
    domain { PN }
    version { $Id$ }
    desc { Sift prime numbers. }
    author { T. M. Parks }
    copyright
    {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { PN library }

    input
    {
	name { input }
	type { int }
    }

    output
    {
	name { output }
	type { int }
    }
    
    go
    {
	StringList prime((input%0).print());
	output%0 = input%0;	// Emit a discovered prime.

	DataFlowStar* star = splice("Filter", "input", "output", &input);
	star->setState("prime", prime);
	star->initState();
	newProcess(star);
    }
}
