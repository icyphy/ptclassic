ident
{
/**************************************************************************
Version identification:
$Id$ 

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Tom Parks
 Date of creation: 1/14/91

 This star emulates a server.  If input events arrive when it is not busy,
 it delays them by the service time.  If they arrive when it is busy,
 it delays them by more.  It must become free, and then serve them.

 The service time can be changed using an addtional input.

**************************************************************************/
}

defstar
{
    name { VarServer }
    domain { DE }
    desc
    {
This star emulates a server.  If input events arrive when it is not
busy, it delays them by the service time (variable).  If they arrive
when it is busy, it delays them by more.  It must become free, and then
serve them. The service time can be changed using an additional input.
    }

    input
    {
	name { input }
	type { anytype }
    }

    input
    {
	name { newServiceTime }
	type { float }
    }

    output
    {
	name { output }
	type { = input }
    }

    defstate
    {
	name { serviceTime }
	type { float }
	default { 1.0 }
	desc { Initial service time. }
	attributes { A_NONCONSTANT | A_SETTABLE }
    }

    constructor
    {
	delayType = TRUE;
    }

    go
    {
	if (newServiceTime.dataNew)
	{
	    serviceTime = float(newServiceTime.get());
	}

	if (input.dataNew)
	{
	   // No overlapped execution. set the time.
	   if (arrivalTime > completionTime)
		completionTime = arrivalTime + double(serviceTime);
	   else
		completionTime += double(serviceTime);

	   Particle& pp = input.get();
           output.put(completionTime) = pp;
	}
    }
}
