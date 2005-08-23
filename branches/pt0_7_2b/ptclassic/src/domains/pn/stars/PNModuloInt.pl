defstar
{
    name { ModuloInt }
    domain { PN }
    version { @(#)PNModuloInt.pl	1.1 2/9/96 }
    desc
    {
Divides the input stream into a stream of numbers divisible by N and
another stream of numbers that are not divisible by N.
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
	name { divisible }
	type { int }
	attributes { P_DYNAMIC }
    }

    output
    {
	name { notDivisible }
	type { int }
	attributes { P_DYNAMIC }
    }

    state
    {
	name { divisor }
	type { int }
	default { 1 }
    }

    go
    {
	PortHole* output;
	if (int(input%0) % int(divisor) == 0) output = &divisible;
	else output = &notDivisible;
	output->receiveData();
	(*output)%0 = input%0;
	output->sendData();
    }
}
