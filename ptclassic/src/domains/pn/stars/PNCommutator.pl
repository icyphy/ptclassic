defstar
{
    name { Commutator }
    domain { PN }
    version { $Id$ }
    desc
    {
Takes N input streams (where N is the number of inputs) and
synchronously combines them into one output stream.  It consumes B
particles from an input (where B is the blockSize), and produces B
particles on the output, then it continues by reading from the next
input.  The first B particles on the output come from the first input,
the next B particles from the next input, etc.
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

    inmulti
    {
	name {input}
	type {ANYTYPE}
	attributes { P_DYNAMIC }
    }

    output
    {
	name {output}
	type {=input}
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
	MPHIter nextPort(input);
	for (int i = input.numberPorts(); i > 0; i--)
	{
	    PortHole& port = *nextPort++;
	    port.receiveData();
	    output.receiveData();
	    for (int j = int(blockSize)-1; j >= 0; j--)
		output%j = port%j;
	    output.sendData();
	}
    }
}
