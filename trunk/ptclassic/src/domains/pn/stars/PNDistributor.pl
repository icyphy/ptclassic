defstar
{
    name { Distributor }
    domain { PN }
    version { $Id$ }
    desc
    {
Takes one input stream and splits it into N output streams, where N is
the number of outputs.  It consumes B input particles, where B =
blockSize, and sends them to the first output.  It conumes another B
input particles and sends them to the next output, etc.
    }
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
	name { input }
	type { ANYTYPE }
	attributes { P_DYNAMIC }
    }

    outmulti
    {
	name { output }
	type { =input }
	attributes { P_DYNAMIC }
    }

    state
    {
	name {blockSize}
	type {int}
	default {1}
	desc {Number of particles in a block.}
    }

    setup
    {
	input.setSDFParams(int(blockSize),int(blockSize)-1);
	output.setSDFParams(int(blockSize),int(blockSize)-1);
    }

    go
    {
	MPHIter nextPort(output);
	for (int i = output.numberPorts(); i > 0; i--)
	{
	    PortHole& port = *nextPort++;
	    input.receiveData();
	    port.receiveData();
	    for (int j = int(blockSize)-1; j >= 0; j--)
		port%j = input%j;
	    port.sendData();
	}
    }
}
