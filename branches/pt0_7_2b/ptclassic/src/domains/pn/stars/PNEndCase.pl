defstar
{
    name { EndCase }
    domain { PN }
    desc
    {
Depending on the "control" particle, consume a particle from one of the
data inputs and send it to the output.  The value of the control
particle should be between zero and N-1, where N is the number of data
inputs.
    }
    version { @(#)PNEndCase.pl	1.4 3/2/95 }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
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

    inmulti
    {
	name { input }
	type { =output }
	attributes { P_DYNAMIC }
    }

    output
    {
	name { output }
	type { anyType }
    }

    go
    {
	StringList name;
	name << "input#" << int(control%0)+1;

	PortHole* port = portWithName(name);
	if (port != NULL)
	{
	    port->receiveData();
	    output%0 = (*port)%0;
	}
	else Error::abortRun (*this, "control value out of range");
    }
}
