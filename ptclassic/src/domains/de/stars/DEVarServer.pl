defstar
{
    name { VarServer }
    domain { DE }
    derivedFrom { Server }
    desc {
This star emulates a server with a variable service time.
If input events arrive when it is idle, they will be serviced
immediately and will be delayed only by the service time.
If input events arrive while another event is being serviced,
they will be queued.  When the server becomes free, it will
service any events waiting in its queue.
    }
    version { @(#)DEVarServer.pl	1.10 06/04/96 }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }

    input {
	name { newServiceTime }
	type { float }
    }

    constructor {
	// state is no longer constant
	serviceTime.clearAttributes(A_CONSTANT|A_SETTABLE);
    }

    go {
	if (newServiceTime.dataNew)
	    serviceTime = newServiceTime.get();

	if (input.dataNew)
	    DEServer::go();
    }
}
