ident
{
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Tom Parks
 Date of creation: 1/11/91

 This star delays its input by an amount given by either the "delay"
 parameter or the "newDelay" input.

**************************************************************************/
}

defstar
{
    name { VarDelay }
    domain { DE }
    desc
    {
Delays its input by an amount given by either the "delay" parameter or
the "newDelay" input.
    }

    input
    {
	name { input }
	type { anytype }
    }

    input
    {
	name { newDelay }
	type { float }
    }

    output
    {
	name { output }
	type { = input }
    }

    defstate
    {
	name { delay }
	type { float }
	default { 1.0 }
	desc { Initial time delay. }
	attributes { A_NONCONSTANT | A_SETTABLE }
    }

    constructor
    {
	delayType = TRUE;
    }

    go
    {
	if (newDelay.dataNew)
	{
	    delay = float(newDelay.get());
	}

	if (input.dataNew)
	{
	    completionTime = arrivalTime + double(delay);
	    Particle& pp = input.get();
	    output.put(completionTime) = pp;
	}
    }
}
