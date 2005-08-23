defstar {
	name {Base}
	domain {HOF}
	desc {
This is a base class containing utility methods shared by HOF stars,
and defining their common functionality.
	}
	htmldoc {
This star provides the base class for a family of <i>higher-order
functions</i> in Ptolemy.
<a name="higher-order functions"></a>
This star cannot be used on its own.
<p>
The HOF domain consists of stars that rewire the galaxy during the
<i>preinitialize</i> phase of setup, and then delete themselves from
the galaxy.
Thus, the scheduler will never see the HOF stars.  Furthermore,
porthole initialization and type resolution occur after preinitialization.
Therefore these stars can be used in any Ptolemy domain;
their semantics are independent of the model of computation.
<p>
This base class contains functions to support rewiring and self-deletion.
Many of the HOF stars replace themselves with one or more instances
of a replacement block.  Additional functions to support that behavior
are in the base class <tt>BaseHiOrdFn</tt>.
	}
	version { @(#)HOFBase.pl	1.18 09/10/99 }
	author { Edward A. Lee, Tom Lane  }
	location { HOF main library }
	copyright {
Copyright (c) 1994-1999 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}

        hinclude {<stdio.h>}
	ccinclude {"Galaxy.h"}
	ccinclude {"InterpGalaxy.h"}
	ccinclude { "SimControl.h" }

	// All HOF stars do their useful work at preinitialization time,
	// and then self-destruct.  HOFBase::preinitialize provides common
	// code needed by all HOF stars.  Derived stars should not override
	// the preinitialize method, but rather the doExpansion method.

	virtual method {
	  name { doExpansion }
	  type { int }
	  code {
	    Error::abortRun(*this, "HOFBase star is not useful on its own");
	    return 0;
	  }
	}

	method {
	    name { preinitialize }
	    access { public }
	    code {
	      // Initialize my states so that they contain valid values.
	      // The preinitialize mechanism doesn't do this by default,
	      // because most stars have no preinitialize behavior and
	      // thus it would be a waste of time to do it for every star.
	      initState();

	      // Don't proceed if initState reported an error.
	      if (SimControl::haltRequested())
		return;

	      // Make sure I have a parent galaxy.
	      Galaxy* mom = idParent();
	      if (!mom) return;

	      // Do the star-specific expansion.
	      if (!doExpansion()) return;

	      // Debugging check: make sure all my portholes are disconnected
	      // and dealiased.  Otherwise doExpansion() screwed up.
	      BlockPortIter pi(*this);
	      PortHole* p;
	      while ((p = pi++) != 0) {
		if (p->far() || p->aliasFrom()) {
		  Error::abortRun(*p,
				  "porthole still connected after expansion");
		  return;
		}
	      }

	      // Delink any aliases pointing at my multiportholes.
	      // There is noplace to reconnect them to, so galaxy MPHs
	      // linked to my portholes become useless after I've preinited.
	      BlockMPHIter mpi(*this);
	      MultiPortHole* mp;
	      while ((mp = mpi++) != 0) {
		mp->clearAliases();
	      }

	      // Tell the parent galaxy to delete me after this preinit pass.
	      mom->deleteBlockAfterInit(*this);
	    }
	}

	method {
	    name { idParent }
	    type { "Galaxy*" }
	    access { protected }
	    code {
	        // Identify the parent galaxy
	        Block *dad = parent();
	        if (!dad || dad->isItAtomic()) {
		    Error::abortRun(*this, "No parent!");
		    return NULL;
	        }
	        return &(dad->asGalaxy());
	    }
	}

	// The following must be called by any HOF star (except Nop) on any
	// multiPortHole if the HOF star's behavior depends on the number of
	// connections in the multiPortHole.  We preinitialize (and thus
	// self-destruct) any HOFNop attached to the MPH.  This is necessary
	// since HOFNop might increase the number of connections, and we
	// can't be sure that the preinitialize pass will visit HOFNop first.
	// Yes, this is a violation of information hiding.  But I believe that
	// the functionality of the Nop star justifies it.  A perhaps logically
	// cleaner solution would involve trying to preinitialize the far end
	// whether it was a Nop or not, but that would require some scheme to
	// prevent infinite recursion.
	//
	// Note that we preinitialize the far end HOFNop even if it is inside
	// a different galaxy (eg, a subgalaxy controlled by a Map star).
	// This would not work reliably for other star types anyway, because
	// the subgalaxy's states may not have been initialized yet.  But
	// HOFNop has no states and thus will never need to reference states
	// of its containing galaxy.
	//
	// The bottom line is that *only* HOFNop (or other star types that
	// might be specifically added to this method) is allowed to increase
	// the number of connections to another star's multiPortHole during
	// preinitialize.  This is ugly but I see no simple way around it.
	//
	// To support use by non-HOF-domain stars that might also want to know
	// the number of their multiport connections at preinit time, we make
	// this a public static method.

	static method {
	    name { initConnections }
	    type { int }
	    arglist { "(MultiPortHole &mph)" }
	    access { public }
	    code {
		// When we find a HOFNop at the far end, we preinitialize it,
		// then break out of the inner loop and re-execute the search
		// from the top.  It is not safe to continue the inner loop
		// since the current MPH list entry has been deleted.
		// This implies that HOFNop *MUST* delete its old connection!
		PortHole *p;
		do {
		    MPHIter next(mph);
		    while ( (p = next++) != 0 ) {
			PortHole *farsideport;
			Block *farstar;
			if ((farsideport = p->far()) &&
			    (farstar = farsideport->parent())) {
			    if (farstar->isA("HOFNop")) {
				farstar->preinitialize();
				// Prevent looping if HOFNop preinit fails
				if (SimControl::haltRequested())
				  return 0;
				// Else assume it's gone, rescan
				break;
			    }
			}
		    }
		} while (p);
		return 1;
	    }
	}

        // Given a pointer pi to one of my input portholes, this
        // method finds the source port connected to that input
        // porthole and reconnects it to the specified destination.
	// Also, any aliases pointing at pi are relinked to the destination.
        // If the destination porthole is a multiporthole, then a
        // new porthole will be instantiated in it.
	method {
	    name { connectInput }
	    type { int }
	    access { protected }
	    arglist { "(PortHole* pi, GenericPort* dest)" }
	    code {
		PortHole* farside = pi->far();
		if (!farside) {
		  Error::abortRun(*pi,"Star is not fully connected");
		  return 0;
		}
		int numdelays = pi->numInitDelays();
		const char* initDelayVals = pi->initDelayValues();
		farside->disconnect();

		// Reconnect farside to specified destination
		connectPorts(*farside,*dest,numdelays,initDelayVals,
			     0, pi);
		return 1;
	    }
	}
        // Given a pointer po to one of my output portholes, this
        // method finds the destination port connected to that output
        // porthole and reconnects it to the specified source porthole.
	// Also, any aliases pointing at po are relinked to the source.
        // If the source porthole is a multiporthole, then a
        // new porthole will be instantiated in it.
	method {
	    name { connectOutput }
	    type { int }
	    access { protected }
	    arglist { "(PortHole* po, GenericPort* source)" }
	    code {
		PortHole* farside = po->far();
		if (!farside) {
		  Error::abortRun(*po,"Star is not fully connected");
		  return 0;
		}
		int numdelays = po->numInitDelays();
		const char* initDelayVals = po->initDelayValues();
		farside->disconnect();

		// Reconnect farside to specified destination
		connectPorts(*source,*farside,numdelays,initDelayVals,
			     po, 0);
		return 1;
	    }
	}
        // Given pointers pi and po to a pair of my input and output
	// portholes, this method reconnects the source port feeding pi
	// to the destination port fed by po.
	// Aliases pointing at pi or po are moved accordingly.
	method {
	    name { crossConnect }
	    type { int }
	    access { protected }
	    arglist { "(PortHole* pi, PortHole* po)" }
	    code {
		PortHole* source = pi->far();
		if (!source) {
		  Error::abortRun(*pi,"Star is not fully connected");
		  return 0;
		}
		PortHole* sink = po->far();
		if (!sink) {
		  Error::abortRun(*po,"Star is not fully connected");
		  return 0;
		}
		int numInDelays = pi->numInitDelays();
		int numOutDelays = po->numInitDelays();
		const char *sourceDelayVals = pi->initDelayValues();
		const char *sinkDelayVals = po->initDelayValues();

		source->disconnect();
		sink->disconnect();

		int numDelays = 0;
		const char* delayVals = 0;
		if (numInDelays > 0 || (sourceDelayVals && *sourceDelayVals)) {
		  numDelays = numInDelays;
		  delayVals = sourceDelayVals;
		  if (numOutDelays > 0 || (sinkDelayVals && *sinkDelayVals)) {
		    Error::warn(*this,
				"Cannot have delays on both input and output."
				" Using input value");
		  }
		} else {
		  numDelays = numOutDelays;
		  delayVals = sinkDelayVals;
		}
		connectPorts(*source,*sink,numDelays,delayVals,
			     po, pi);
		return 1;
	    }
	}

	// Make a connection between two portholes.
	// Reattach any aliases pointing to oldSource or oldSink to
	// the newly connected (possibly newly created!) portholes.
	// Finally, register the connection with the parent galaxy
	// so that delays get initialized when the galaxy is reinitialized.
	method {
	    name { connectPorts }
	    access { protected }
	    arglist { "(GenericPort& source, GenericPort& sink, int numberDelays, const char* initDelayValues, PortHole* oldSource, PortHole* oldSink)" }
	    code {
		// First, expand multiporthole names to individual portholes.
		// This is needed to ensure that the delay will be reattached
		// to the right place...
		PortHole& realSource = source.newConnection();
		PortHole& realSink = sink.newConnection();
		// Make the connection.
		realSource.connect(realSink,numberDelays,initDelayValues);
		// Swing the aliases.
		// This must be done BEFORE trying to register the delay,
		// else the portholes may not have any names visible to
		// a common upper-level galaxy.
		if (oldSource) oldSource->moveMyAliasesTo(realSource);
		if (oldSink) oldSink->moveMyAliasesTo(realSink);
		// See if we have a delay to register.
		if (initDelayValues && *initDelayValues) {
		  registerDelay(realSource, realSink, initDelayValues);
		} else if (numberDelays > 0) {
		  // construct an old-style delay string, of the form *n
		  // necessary because InterpGalaxy::registerInit doesn't
		  // save old-style delays as such (yech)
		  char delayString[32];
		  sprintf(delayString, "*%d", numberDelays);
		  registerDelay(realSource, realSink, hashstring(delayString));
		}
	    }
	}

	// Find a galaxy to register the delay-recreation request in.
	// My immediate parent may not be able to do the job,
	// because one or both portholes might belong to stars outside
	// my immediate galaxy.  So we search upwards to find an InterpGalaxy
	// that knows about both ends.  (In the case where the two
	// portholes are at different hierarchy depths, there should
	// be an alias at the upper level for the lower-level port,
	// if we've done our alias-munging correctly.)
	method {
	    name { registerDelay }
	    access { protected }
	    arglist { "(GenericPort& source, GenericPort& sink, const char* initDelayValues)" }
	    code {
	      int sawIG = FALSE;
	      for (Block* gal = parent(); gal; gal = gal->parent()) {
		if (gal->isA("InterpGalaxy")) {
		  InterpGalaxy* igal = (InterpGalaxy*) gal;
		  sawIG = TRUE;
		  const char *blockName, *portName;
		  if (! igal->getPortHoleName(source, blockName, portName))
		    continue;
		  if (! igal->getPortHoleName(sink, blockName, portName))
		    continue;
		  igal->registerInit("C", &source, initDelayValues, &sink);
		  return;	// success exit
		}
	      }
	      // If no parent is an InterpGalaxy, assume we are in a context
	      // where reinitialization is not a concern.
	      // Otherwise, better gripe.
	      if (sawIG) {
		Error::warn(*this,
			    "Unable to register delay for reinitialization; "
			    "re-running will not work.");
	      }
	    }
	}

}
