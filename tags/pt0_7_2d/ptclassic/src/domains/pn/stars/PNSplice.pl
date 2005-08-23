defstar
{
    name { Splice }
    domain { PN }
    version { @(#)PNSplice.pl	1.11 3/29/96 }
    desc { Base class for stars that splice in other stars. }
    author { T. M. Parks }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { PN library }

    hinclude { "PNThread.h", "InterpGalaxy.h" }
    ccinclude { "Geodesic.h" }

    protected
    {
	InterpGalaxy* gal;	// Container for dynamically created stars.
	int spliceCount;	// Number of spliced stars.
	ThreadList* threads;	// Container for dynamically created threads.
    }

    conscalls
    {
	gal(0), spliceCount(0), threads(0)
    }

    destructor
    {
	LOG_DEL; delete threads;
	LOG_DEL; delete gal;
    }

    method
    {
	type { "DataFlowStar*" }
	name { splice }
	arglist { "(const char* starClass, const char* aliasPort, const char* connectPort, PortHole* port)" } 
	code
	{
	    StringList starName;
	    starName << starClass << "-splice-" << ++spliceCount;

	    if (!gal)	// Create galaxy to hold spliced stars.
	    {
		LOG_NEW; gal = new InterpGalaxy("splice","PN");
		gal->setNameParent("splice", this);
	    }

	    gal->addStar(starName, starClass);
	    DataFlowStar* star = (DataFlowStar*)gal->blockWithName(starName);
	    if (star == NULL)
	    {
		Error::abortRun(*this, "could not splice in ", starClass);
		return star;
	    }

	    // The galaxy's port list is used to keep track of the
	    // original conection.
	    if (!gal->portWithName(port->name()))
		gal->alias(port->name(), starName, aliasPort);

	    // Make connections.
	    PortHole* alias = star->portWithName(aliasPort);
	    PortHole* con = star->portWithName(connectPort);
	    PortHole* far = port->far();
	    
	    port->disconnect(FALSE);	// Do not delete the geodesic.
	    if (port->isItInput()) far->geo()->setDestPort(*alias);
	    else far->geo()->setSourcePort(*alias);
	    port->connect(*con, 0);

	    // Locking must be enabled AFTER initialization so that
	    // the plasma will exist.
	    PNMonitor proto;
	    star->initialize();
	    alias->enableLocking(proto);
	    con->enableLocking(proto);

	    return star;
	}
    }

    method
    {
	name { unsplice }
	code
	{
	    // Delete any processes that were created.
	    LOG_DEL; delete threads; threads = NULL;

	    // Step through the ports to reestablish the
	    // original connections.
	    BlockPortIter nextPort(*this);
	    PortHole* port;
	    while ((port = nextPort++) != NULL)
	    {
		PortHole* alias = gal->portWithName(port->name());
		if (alias == NULL) continue;

		PortHole* aFar = alias->far();
		PortHole* pFar = port->far();
		
		port->disconnect();
		pFar->disconnect(FALSE);
		alias->disconnect(FALSE);

		if (port->isItInput())
		{
		    aFar->geo()->setDestPort(*port);
		}
		else
		{
		    aFar->geo()->setSourcePort(*port);
		}
	    }

	    LOG_DEL; delete gal;
	    gal = 0;
	    spliceCount = 0;
	}
    }

    method
    {
	name { newProcess }
	arglist { "(DataFlowStar* star)" }
	code
	{
	    if (!threads)
	    {
		LOG_NEW; threads = new ThreadList;
	    }
	    LOG_NEW; DataFlowProcess* p = new DataFlowProcess(*star);
	    threads->add(p);
	    p->initialize();
	}
    }

    setup
    {
	if (gal) unsplice();
    }

    wrapup
    {
	if (gal)
	{
	    gal->wrapup();
	    unsplice();
	}
    }
}
