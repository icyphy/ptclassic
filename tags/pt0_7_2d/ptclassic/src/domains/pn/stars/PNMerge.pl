defstar
{
    name { Merge }
    domain { PN }
    version { @(#)PNMerge.pl	1.5 03/29/96 }
    desc { Merge two increasing sequences, eliminating duplicates. }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { PN library }

    input
    {
	name { a }
	type { int }
	attributes { P_DYNAMIC }
    }

    input
    {
	name { b }
	type { int }
	attributes { P_DYNAMIC }
    }

    output
    {
	name { output }
	type { int }
	attributes { P_DYNAMIC }
    }

    begin
    {
	// Read both inputs the first time.
	a.receiveData();
	b.receiveData();
    }

    go
    {
	output.receiveData();	// Initialize the output.

	if (int(a%0) < int(b%0))
	{
	    output%0 = a%0;
	    output.sendData();
	    a.receiveData();
	}
	else if (int(a%0) > int(b%0))
	{
	    output%0 = b%0;
	    output.sendData();
	    b.receiveData();
	}
	else	// Remove duplicates.
	{
	    output%0 = a%0;
	    output.sendData();
	    a.receiveData();
	    b.receiveData();
	}
    }
}
