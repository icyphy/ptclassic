ident
{
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1991 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  T. M. Parks
 Date of creation: 1/14/91

 This star emulates a server with a variable service time.
 If input events arrive when it is idle, they will be serviced
 immediately and will be delayed only by the service time.
 If input events arrive while another event is being serviced,
 they will be queued.  When the server becomes free, it will
 service any events waiting in its queue.

**************************************************************************/
}

defstar
{
    name { VarServer }
    domain { DE }
    derivedFrom { Server }
    desc
    {
This star emulates a server with a variable service time.
If input events arrive when it is idle, they will be serviced
immediately and will be delayed only by the service time.
If input events arrive while another event is being serviced,
they will be queued.  When the server becomes free, it will
service any events waiting in its queue.
    }
    version { $Id$ }
    author { T. M. Parks }
    copyright { 1991 The Regents of the University of California }

    input
    {
	name { newServiceTime }
	type { float }
    }

    constructor
    {
	// state is no longer constant
	serviceTime.clearAttributes(A_CONSTANT|A_SETTABLE);
    }

    go
    {
	if (newServiceTime.dataNew)
	    serviceTime = newServiceTime.get();

	if (input.dataNew)
	    DEServer::go();
    }
}
