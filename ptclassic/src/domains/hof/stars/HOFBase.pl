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
This star should not be used on its own.
<a name="higher-order functions"></a>
A higher-order function is a function that takes a function as
an argument and/or returns a function.
Stars and galaxies in Ptolemy have two kinds of arguments:
signals and parameters.
The higher-order functions supported by this base class
take functions as parameters, not signals.
The basic mechanism is that a star or galaxy is statically
specified, and the higher-order star replaces itself with
one or more instances of the specified replacement block.
	}
	version { $Id$ }
	author { E. A. Lee  }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	protected {
	  static int count;	// for assigning unique identifiers
	  Block* myblock;
	}
	code {
		int HOFBase::count = 0;
	}
	location { HOF main library }
	ccinclude {"Galaxy.h"}
	ccinclude {"InterpGalaxy.h"}
	ccinclude { "InfString.h" }
	ccinclude { "KnownBlock.h" }
	ccinclude { "ptk.h" }
	// Since block states are not yet initialized during the preinitialize
	// pass, any HOF star that wants to access state values should be sure
	// to call this.
	method {
	    name { preinitialize }
	    access { public }
	    code {
		initState();
	    }
	}

	method {
	    name { idParent }
	    type { "Galaxy*" }
	    access { protected }
	    arglist { "()" }
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
	// since HOFNop might increase the number of connections, and we can't
	// be sure that the preinitialize pass will visit HOFNop first.
	// Yes, this is a violation of information hiding.  But I believe that
	// the functionality of the Nop star justifies it.  A perhaps logically
	// cleaner solution would involve trying to preinitialize the far end
	// whether it was a Nop or not, but that would require some scheme to
	// prevent infinite recursion.
	//
	// However, we do *not* preinitialize the far end HOFNop if it is
	// inside a different galaxy (ie, a subgalaxy).  That would cause our
	// aliased connection to it to be converted into multiple connections
	// to elements of the subgalaxy, and Ptolemy's current aliasing scheme
	// is too simplistic to allow us to cope.  This means that the number
	// of connections to a subgalaxy's multiport must be specified by an
	// explicit bus marker in our galaxy, even when it might have been
	// deducible by expanding a HOFNop just inside the subgalaxy boundary.
	// Fixing this looks like much more trouble than it's worth.
	method {
	    name { initConnections }
	    type { "void" }
	    access { protected }
	    arglist { "(MultiPortHole &mph)" }
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
			    (farstar = farsideport->parent()) &&
			    (farstar->parent() == parent()) &&
			    (farstar->isA("HOFNop"))) {
				farstar->preinitialize();
				break;
			}
		    }
		} while (p);
	    }
	}

	virtual method {
	    name { createBlock }
	    type { "Block*" }
	    access { protected }
	    arglist { "(Galaxy& mom, const char* blockname, const char* where_defined)" }
	    code {
	        // Create the star and install in the galaxy
	        if (!KnownBlock::find(blockname, mom.domain())) {

		    // Failed to find star master.  Try compiling the
		    // facet specified in the where_defined argument
		    InfString command = "ptkOpenFacet ";
		    command += where_defined;
		    if(Tcl_GlobalEval(ptkInterp, (char*)command) == TCL_ERROR) {
			Error::abortRun(*this,
		            "Unable to open the where_defined facet: ",
		            where_defined);
			return NULL;
		    }
		    command = "ptkCompile ";
		    command += ptkInterp->result;
		    if(Tcl_GlobalEval(ptkInterp, (char*)command) == TCL_ERROR) {
			Error::abortRun(*this,
		            "Unable to compile the where_defined facet: ",
		            where_defined);
			return NULL;
		    }
	        }
		// Should now have a master of the star -- clone it
		Block *block = KnownBlock::clone(blockname, mom.domain());
		if (!block) {
		    Error::abortRun(*this,
				    "Unable to create an instance of block: ",
				    blockname);
		    return NULL;
		}
		// Set the target in the new block.
		// This may no longer be necessary, but it can't hurt.
		if (target()) block->setTarget(target());

		// Choose a unique name for the block
	        StringList instancename = "HOF_";
		instancename += blockname;
	        instancename += count++;
		// The following amounts to a small memory leak.
		// FIXME: could reduce leakage by not insisting on *global*
		// uniqueness of the generated name.  What about using my
		// own block's name plus a local counter?? 
	        const char* instance = hashstring(instancename);

		// Add to parent galaxy.  Note that since new blocks are
		// added at the end of the block list, this block will get
		// a preinitialize call later on.
		mom.addBlock(*block,instance);

		return block;
	    }
	}
        // Given a pointer pi to one of my input portholes, this
        // method finds the source port connected to that input
        // porthole and reconnects it to the specified destination.
        // If the destination porthole is a multiporthole, then a
        // new porthole will be instantiated.
	method {
	    name { connectInput }
	    type { int }
	    access { protected }
	    arglist { "(PortHole* pi, GenericPort* dest)" }
	    code {
		GenericPort *gp = aliasPointingAt(pi);

		PortHole* farside = pi->far();
		if (!farside) {
		  Error::abortRun(*this,"Star is not fully connected");
		  return 0;
		}
		int numdelays = pi->numInitDelays();
		const char* initDelayVals = pi->initDelayValues();
		farside->disconnect();

		connectPorts(*farside,*dest,numdelays,initDelayVals);
		fixAliases(gp,pi,dest);
		return 1;
	    }
	}
        // Given a pointer po to one of my output portholes, this
        // method finds the destination port connected to that output
        // porthole and reconnects it to the specified source porthole.
        // If the source porthole is a multiporthole, then a
        // new porthole will be instantiated.
	method {
	    name { connectOutput }
	    type { int }
	    access { protected }
	    arglist { "(PortHole* po, GenericPort* source)" }
	    code {
		GenericPort *gp = aliasPointingAt(po);

		PortHole* farside = po->far();
		if (!farside) {
		  Error::abortRun(*this,"Star is not fully connected");
		  return 0;
		}
		int numdelays = po->numInitDelays();
		const char* initDelayVals = po->initDelayValues();
		farside->disconnect();

		connectPorts(*source,*farside,numdelays,initDelayVals);
		fixAliases(gp,po,source);
		return 1;
	    }
	}
	// Make a connection between two portholes.
	// Register the connection with the parent galaxy so that delays get
	// initialized when the galaxy is reinitialized.
	method {
	    name { connectPorts }
	    access { protected }
	    arglist { "(GenericPort& source, GenericPort& sink, int numberDelays, const char* initDelayValues)" }
	    code {
		source.connect(sink,numberDelays,initDelayValues);
		if (parent()->isA("InterpGalaxy")) {
		  if (initDelayValues && *initDelayValues) {
		    ((InterpGalaxy*)parent())->registerInit("C",
				source.parent()->name(),
				source.name(),
				initDelayValues,
				sink.parent()->name(),
				sink.name());
		  } else if (numberDelays > 0) {
		    // construct an old-style delay string, of the form *n
		    // necessary because registerInit doesn't save old-style
		    // delays explicitly (yech)
		    char delayString[32];
		    sprintf(delayString, "*%d", numberDelays);
		    ((InterpGalaxy*)parent())->registerInit("C",
				source.parent()->name(),
				source.name(),
				hashstring(delayString),
				sink.parent()->name(),
				sink.name());
		  }
		}
	    }
	}
	// Find a port (if any) with an alias pointing to po.
	// If there is none, check to see whether po is a port in
	// a MultiPortHole that has an alias pointing to it.
	// Return a pointer to the generic port with the alias, or zero.
	method {
	  name { aliasPointingAt }
	  type { "GenericPort*" }
	  access { protected }
	  arglist { "(PortHole *po)" }
	  code {
	    GenericPort *gp = po->aliasFrom();
	    if (gp == 0) {
	      // Check for a multiporthole alias
	      MultiPortHole *mph = po->getMyMultiPortHole();
	      if (mph != 0)
		gp = mph->aliasFrom();
	    }
	    return gp;
	  }
	}
	// Given a port gp with an alias to another port po,
	// change its alias to point to source and clear the aliases in po.
	method {
	  name { fixAliases }
	  type { void }
	  access { protected }
	  arglist { "(GenericPort *gp, PortHole* po, GenericPort *source)" }
	  code {
		// Fix aliases if any
		if (gp != 0) {
		  // The following prevents a later destruction of
		  // the po porthole from undoing the alias fix below.
		  if (po) po->clearAliases();
		  // Note that this can have the effect of making a
		  // MultiPortHole have a PortHole as an alias
		  gp->setAlias(*source);
		}
	  }
	}
	// Given a parameter name and value, the following method will set
	// the corresponding parameter of the specified block.  If the
	// parameter name is of the form name(number), then the number must
	// match the instanceno argument, or the method does nothing.
	method {
	    name { setParameter }
	    type { int }
	    access { protected }
	    arglist { "(Block* block, int instanceno, const char* parameter_name, const char* parameter_value)" }
	    code {
	      // First step is to determine whether the parameter
	      // name is of the form xxx(n) or xxx(n)=, in which case, it only
	      // applies to this instance if n == instanceno.
	      // Use a regular express to parse the form xxx(n).

	      InfString command =
		"regexp {^ *([^( =]*) *\\( *([0-9]+) *\\) *=? *$} ";
	      command += parameter_name;
	      command += " junk parameter_name instance_index";
	      Tcl_Eval(ptkInterp, (char*)command);

	      // Check for regular expression match
	      if (!strcmp("1",ptkInterp->result)) {

		// The regular expression matched, so we have xxx(n)
		// First convert the ascii instance number to numeric
		// Can safely ignore unexpected Tcl failures,
		// since givenInstNo will end up being zero

		int givenInstNo = 0;
		command = "instance_index";
		char* instance_index =
		  Tcl_GetVar(ptkInterp,command,(int)NULL);
		Tcl_GetInt(ptkInterp, instance_index, &givenInstNo);

		// If the instance numbers don't match,
		// there is nothing more to do.
		if (!(instanceno == givenInstNo)) return 1;

		// If the instance numbers match, then reset
		// parameter_name and proceed with substitutions
		command = "parameter_name";
		parameter_name = Tcl_GetVar(ptkInterp,command,(int)NULL);
		if (!parameter_name) {
		  Error::abortRun(*this,
				  "Unexpected error setting parameter_name!");
		  return 0;
		}
		
	      }

	      // Whether the regular expression matched or not,
	      // replace references to instance number in the
	      // parameter value. Use Tcl to do this, since it has
	      // such convenient string manipulation functions.

	      command = "regsub -all instance_number {";
	      command += parameter_value;
	      command += "} ";
	      command += instanceno;
	      command += " state_value";
	      if(Tcl_Eval(ptkInterp, (char*)command)
		 == TCL_ERROR) {
		Error::abortRun(*this,
				"Substitution of instance number failed:",
				ptkInterp->result);
		return 0;
	      }

	      // Set the state
	      command = "state_value";
	      // The call to hashstring makes this string persistent
	      // Hack alert: This is really a small memory leak.
	      if(!block->setState(parameter_name,
				  hashstring(Tcl_GetVar(ptkInterp,command,(int)NULL)))) { 
		Error::abortRun(*this,
				"Bad name in parameter map: ", parameter_name);
		return 0;
	      }
	      return 1;
	    }
	}
	// This method returns FALSE if any of the portholes of block
	// are not connected to star.
	method {
	  name { restrictConnectivity }
	  type { "int" }
	  access { protected }
	  arglist { "(Block *block, Star *star)" }
	  code {
	    BlockPortIter bpi(*block);
	    PortHole *p;
	    while ( (p = bpi++) ) {
	      PortHole *far = p->far();
	      if (!far) {
		// This might be a galaxy porthole, which should return something,
		// but doesn't.  Find the corresponding real porthole.
		GenericPort &gp = p->realPort();
		if (gp.isA("PortHole")) { // is this test necessary?
		  p = (PortHole*)(&gp);
		  far = p->far();
		}
	      }
	      // Ignore the porthole if far is still NULL,
	      // or if the porthole is hidden
	      if (far && !hidden(*far) && (far->parent() != star)) {
		Error::abortRun(*this,
			    "Sorry, replacement block can only be a single block");
		return FALSE;
	      }
	    }
	    return TRUE;
	  }
	}
	// This method returns the generic port at the top level
	// of the alias chain for the given porthole.  In other
	// words, given a star porthole, it will find the highest
	// galaxy porthole aliased to the star porthole.
	method {
	  name { findTopGenericPort }
	  type { "GenericPort*" }
	  access { protected }
	  arglist { "(PortHole *ph)" }
	  code {
	    MultiPortHole *mph;
	    GenericPort *gp, *gpt;
	    if ( (mph = ph->getMyMultiPortHole()) ) {
	      gp = mph;
	    } else {
	      gp = ph;
	    }
	    // Get the top-level port that we are connected to
	    while ( (gpt = gp->aliasFrom()) ) {
	      gp = gpt;
	    }
	    // If the multiporthole did not have an aliasFrom
	    // porthole, it could still be that the original
	    // porthole did.
	    if (gp == mph) {
	      gp = ph;
	      while ( (gpt = gp->aliasFrom()) ) {
		gp = gpt;
	      }
	      // If the original porthole also did not have an aliasFrom,
	      // then make sure to return a pointer to its mph.
	      if (gp == ph) gp = mph;
	    }
	    return gp;
	  }
	}
	method {
	  name { connectError }
	  type { "void" }
	  access { protected }
	  arglist { "()" }
	  code {
	    Error::abortRun(*this,
			    "Star is not fully connected");
	  }
	}
	// Given a PortHole*, this method disconnects it from
	// whatever it is connected to, and returns the top-level
	// GenericPort* (i.e. galaxy port) for the far side of
	// that connection.  As a side effect, if the protected
	// member "myblock" is NULL, it sets it to point to the
	// block on the far side of the connection.  If it is non-NULL,
	// it checks to see that the block on the far side is
	// exactly the same as the one pointed to by "myblock."
	// If any error occurs, NULL is returned.
	method {
	  name { breakConnection }
	  type { "GenericPort*" }
	  access { protected }
	  arglist { "(PortHole* peo)" }
	  code {
	    PortHole* dest = peo->far();
	    if (dest == 0) {
	      connectError();
	      return NULL;
	    }
	    GenericPort *destgp = findTopGenericPort(dest);
	    if (!myblock) {
	      myblock = destgp->parent();
	      if (!restrictConnectivity(myblock,this)) return NULL;
	    } else {
	      if (myblock != destgp->parent()) {
		Error::abortRun(*this,
				"Sorry, only a single replacement block"
				" is supported at this time.");
		return NULL;
	      }
	    }
	    dest->disconnect();
	    return destgp;
	  }
	}
}
