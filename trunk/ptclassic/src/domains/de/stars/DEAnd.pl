defstar
{
    name { And }
    domain { DE } 
    desc { Outputs the logical AND of the inputs. }
    version { $Id$ }
    author { T. M. Parks }
    copyright { 1991 The Regents of the University of California }
    location { DE main library }

    inmulti
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
	InDEMPHIter in(input);
	InDEPort *inPort;
	int x;

	// compute bit-wise AND of all inputs
	for(x = TRUE, inPort = in++; inPort != NULL; inPort = in++)
	{
	    x = x && int(inPort->get());
	}
	output.put(arrivalTime) << x;
    }
}
