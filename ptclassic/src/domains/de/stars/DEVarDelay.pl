ident
{
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  T. M. Parks
 Date of creation: 1/11/91

 This star delays its input by a variable amount.
 The "delay" parameter gives the initial delay,
 and the delay is changed using the "newDelay" input.

**************************************************************************/
}

defstar
{
    name { VarDelay }
    domain { DE }
    derivedFrom { Delay }
    descriptor
    {
This star delays its input by a variable amount.
The "delay" parameter gives the initial delay,
and the delay is changed using the "newDelay" input.
    }
    version { $Id$ }
    author { T. M. Parks }
    copyright { 1991 The Regents of the University of California }

    input
    {
	name { newDelay }
	type { float }
    }

    constructor
    {
	// state is no longer constant
	delay.clearAttributes(A_CONSTANT);
    }

    go
    {
	if (newDelay.dataNew)
	    delay = newDelay.get();

	if (input.dataNew)
	    DEDelay::go();
    }
}
