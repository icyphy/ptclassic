defstar
{
    name { DelayInt }
    domain { PN }
    version { $Id$ }
    desc
    {
    An initializable delay line.
    }
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
	attributes { P_DYNAMIC }
    }

    output
    {
	name { output }
	type { int }
	attributes { P_DYNAMIC }
    }

    state
    {
	name { init }
	type { intArray }
	default { 0 }
	desc { Initial values in the delay line.}
    }

    go
    {
	for (int i = 0; i < init.size(); i++)
	{
	    // Notice that the output is produced before the input is consumed.
	    output.receiveData();
	    output%0 << init[i];
	    output.sendData();

	    input.receiveData();
	    init[i] = input%0;
	}
    }
}
