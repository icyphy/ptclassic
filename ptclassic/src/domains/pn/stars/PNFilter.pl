defstar
{
    name { Filter }
    domain { PN }
    version { @(#)PNFilter.pl	1.4 3/7/96 }
    desc { Filter out multiples of a prime number. }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
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
	attributes { P_DYNAMIC }
    }

    state
    {
	name { prime }
	type { int }
	default { 2 }
	desc { Prime number to filter. }
    }

    go
    {
	int N = input%0;
	int P = prime;
	
	if (N%P) 
	{
	    output.receiveData();
	    output%0 = input%0;
	    output.sendData();
	}
    }
}
