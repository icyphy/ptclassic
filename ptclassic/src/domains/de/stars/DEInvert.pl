defstar
{
    name { Invert }
    domain { DE } 
    desc { Outputs the logical complement of the input. }
    version { $Id$ }
    author { T. M. Parks }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { DE main library }

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
	output.put(arrivalTime) << !int(input.get());
    }
}
