defstar
{
    name { Invert }
    domain { DE } 
    desc { Outputs the logical complement of the input. }
    version { $Id$ }
    author { T. M. Parks }
    copyright { 1991 The Regents of the University of California }
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
