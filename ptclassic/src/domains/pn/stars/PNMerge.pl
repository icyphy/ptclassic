defstar
{
    name { Merge }
    domain { PN }
    version { $Id$ }
    desc { Merge two increasing sequences, eliminating duplicates. }
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
    }

    protected
    {
	int getA:1;	// Should we get data from port a?
	int getB:2;	// Should we get data from port b?
    }

    begin
    {
	// Read both inputs the first time.
	getA = getB = TRUE;
    }

    go
    {
	if (getA) a.receiveData();
	if (getB) b.receiveData();
	getA = getB = FALSE;

	if (a%0 < b%0)
	{
	    output%0 = a%0;
	    getA = TRUE;
	}
	else if (a%0 > b%0)
	{
	    output%0 = b%0;
	    getB = TRUE;
	}
	else	// Remove duplicates.
	{
	    output%0 = a%0;
	    getA = getB = TRUE;
	}
    }
}
