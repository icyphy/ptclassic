defstar
{
    name { LastOfN }
    domain { PN }
    desc
    {
Given a control input with integer value N,
consume N particles from the data input and
produce only the last of these at the output.
    }
    version { $Id$ }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { PN library }

    input
    {
	name { control }
	type { int }
    }

    input
    {
	name { input }
	type { anyType }
	attributes { P_DYNAMIC }
    }

    output
    {
	name { output }
	type { =input }
	attributes { P_DYNAMIC }
    }

    go
    {
	int N = int(control%0);

	if (N > 0)
	{
	    for (int i = 0; i < N; i++)
		input.receiveData();
	    output%0 = input%0;
	    output.sendData();
	}
    }
}
