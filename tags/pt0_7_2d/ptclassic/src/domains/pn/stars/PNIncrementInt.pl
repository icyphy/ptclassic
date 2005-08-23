defstar
{
    name { IncrementInt }
    domain { PN }
    version { @(#)PNIncrementInt.pl	1.1 2/9/96 }
    desc
    {
    Increment the input by a constant.
    }
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
    }

    state
    {
	name { increment }
	type { int }
	default { 0 }
	desc { Increment input by this amount.}
    }

    go
    {
	output%0 << int(input%0) + int(increment);
    }
}
