defstar {
	name {BaseHiOrdFn}
	domain {HOF}
	desc {
This is a base class containing utility methods shared by HOF stars,
and defining their common functionality.
	}
	explanation {
This star provides the base class for a family of \fIhigher-order
functions\fR in Ptolemy.
.IE "higher-order functions"
A higher-order function is a function that takes a function as
an argument and/or returns a function.  Stars and galaxies in
Ptolemy have two kinds of arguments: signals and parameters.
The higher-order functions supported by this base class
take functions as parameters, not signals.
The basic mechanism is that a star or galaxy is statically
specified, and the higher-order star replaces itself with
one or more instances of the specified replacement block.
.pp
The name of the replacement block is given by the \fIblockname\fR
parameter.  If the replacement block is a galaxy,
then the \fIwhere_defined\fR parameter gives the full
name (including the full path) of the definition of the galaxy.
This path name may (and probably should) begin with the environment
.EQ
delim off
.EN
variable $PTOLEMY or ~username.
.EQ
delim $$
.EN
Currently, this must be an oct facet, although in the
future, other specifications (like ptcl files) may be allowed.
The oct facet should contain the replacement galaxy,
or it could simply be the facet that defines the replacement galaxy.
If the replacement block is a built-in star, then there
is no need to give a value to the \fIwhere_defined\fR parameter.
.pp
For all higher-order stars derived from this base class,
the replacement blocks are substituted for the higher-order star
at setup time.  Thus, the scheduler will never see the higher-order
star.  For this reason, these stars can be used in any Ptolemy domain,
since their semantics are independent of the model of computation.
.pp
The stars derived from this base class differ mostly in the way
that replacement blocks get wired into the graph.
They share a common mechanism for specifying the value
of parameters in the replacement block.
.UH "Setting parameter values"
.pp
The \fIparameter_map\fR parameter can be used to set parameter values
for the replacement blocks.  There are four acceptable forms:
.(c
name value
name(number) value
name = value
name(number) = value
.)c
There should be no spaces between "name" and "(number)", and the name
cannot contain spaces, "=", or "(".
In all cases, \fIname\fR is the name of a parameter in the
replacement block.
In the first case, the value is applied to all instances
of the replacement block.
In the second case, it is applied only to the specified instance
number (which starts with 1).  The third and fourth cases just
introduce an optional equal sign, for readability.
If the "=" is used, there must be spaces around it.
.pp
The value can be any usual Ptolemy expression for giving the value
of a parameter.  However, if the string "instance_number" appears
anywhere in the expression, it will be replaced with the instance
number of the replacement block.  Note that it need not be a
separate token.  For example, the value "xxxinstance_numberyyy" will
become "xxx9yyy" for instance number 9.
After this replacement, the value is evaluated using the usual
Ptolemy expression evaluator for initializing states.
	}
	version {$Id$}
	author { E. A. Lee  }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	defstate {
		name {blockname}
		type {string}
		default {"Gain"}
		desc {The name of the replacement block}
	}
	defstate {
		name {where_defined}
		type {string}
		default {""}
		desc {
The full path and facet name for the definition of blockname.
		}
	}
	defstate {
		name {parameter_map}
		type {stringarray}
		default {""}
		desc {The mapping of parameters}
	}
	protected {
	  static int count;
	  Block* myblock;
	}
	code {
		int HOFBaseHiOrdFn::count = 0;
	}
	location { HOF main library }
	ccinclude {"Galaxy.h"}
	ccinclude {"InterpGalaxy.h"}
	ccinclude { "InfString.h" }
	ccinclude { "KnownBlock.h" }
	ccinclude { "ptk.h" }
	ccinclude { "SimControl.h" }
	setup {
	    // Since it is so easy to get into an infinite loop
	    // (just name as your block a galaxy that you are within),
	    // process Tk events, and check for a halt request.
	    // Note that this alone prevents the use of this star in ptcl,
	    // which does not have Tk.
	    // FIXME: This should be done some other way.
	    Tk_DoOneEvent(TK_DONT_WAIT|TK_ALL_EVENTS);
	    if (SimControl::haltRequested()) {
		Error::abortRun(*this, "Initialization aborted");
		return;
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
	// multiPortHoles so that if any HOF star is connected to the multiPortHole,
	// its reconnections get done before the HOF star proceeds.
	// Yes, this is a violation of information hiding.  But I believe that
	// the functionality of the Nop star justifies it.
	ccinclude { "HOFNop.h" }
	method {
	    name { initConnections }
	    type { "void" }
	    access { protected }
	    arglist { "(MultiPortHole &mph)" }
	    code {
	      MPHIter next(mph);
	      PortHole *p, *farsideport;
	      Block *farstar;
	      while (p = next++) {
		if ((farsideport = p->far()) &&
		    (farstar = farsideport->parent()) &&
		    (farstar->isA("HOFNop"))) {
		      ((HOFNop*)farstar)->reconnect();
		    }
	      }
	    }
	  }
	virtual method {
	    name { createBlock }
	    type { "Block*" }
	    access { protected }
	    arglist { "(Galaxy& mom)" }
	    code {
	        // Create the star and install in the galaxy
	        if (!KnownBlock::find((const char*)blockname, mom.domain())) {

		    // Failed to find star master.  Try compiling the
		    // facet specified in the where_defined parameter.
		    InfString command = "ptkOpenFacet ";
		    command += (const char*)where_defined;
		    if(Tcl_GlobalEval(ptkInterp, (char*)command) == TCL_ERROR) {
			Error::abortRun(*this,
		            "Unable to open the where_defined facet: ",
		            (const char*)where_defined);
			return NULL;
		    }
		    command = "ptkCompile ";
		    command += ptkInterp->result;
		    if(Tcl_GlobalEval(ptkInterp, (char*)command) == TCL_ERROR) {
			Error::abortRun(*this,
		            "Unable to compile the where_defined facet: ",
		            (const char*)where_defined);
			return NULL;
		    }
	        }
		// Should now have a master of the star -- clone it
		Block *block = KnownBlock::clone((const char*)blockname,
						 mom.domain());
		if (!block) {
		    Error::abortRun(*this,
			"Unable to create an instance of block: ",
			(const char*)blockname);
		    return NULL;
		}

		// Choose a name for the block
	        StringList instancename = "HOF_";
		instancename += (const char*)blockname;
	        instancename += count++;
	        const char* instance = hashstring(instancename);

		mom.addBlock(*block,instance);
		return block;
	    }
	}
        // Given a pointer pi to one of my input portholes, this
        // method finds the source port connected to that input
        // porthole and reconnects it to the specified destination.
        // If the destination porthole is a multiporthole, then a
        // new porthole will be instatiated.
	method {
	    name { connectInput }
	    type { int }
	    access { protected }
	    arglist { "(PortHole* pi, GenericPort* dest)" }
	    code {
		GenericPort *gp;
		const char* initDelayVals;
		PortHole* farside = pi->far();
		if (!farside) {
		  Error::abortRun(*this,"Star is not fully connected");
		  return 0;
		}
		int numdelays = pi->numInitDelays();
		initDelayVals = pi->initDelayValues();
		farside->disconnect();

		farside->connect(*dest,numdelays,initDelayVals);
		if(parent()->isA("InterpGalaxy") && 
		   ((numdelays > 0) || (initDelayVals && *initDelayVals))) {
		  ((InterpGalaxy*)parent())->registerInit("C",
							  farside->parent()->name(),
							  farside->name(),
							  initDelayVals,
							  dest->parent()->name(),
							  dest->name());
		}
		// Fix aliases if any
		if ((gp = pi->aliasFrom()) != 0) {
		    gp->setAlias(*dest);
		}

		// Since output portholes are responsible for initializing
		// the geodesics, call initialize for the farside port
		farside->initialize();
		return 1;
	    }
	}
        // Given a pointer po to one of my output portholes, this
        // method finds the destination port connected to that output
        // porthole and reconnects it to the specified source porthole.
        // If the source porthole is a multiporthole, then a
        // new porthole will be instatiated.
	method {
	    name { connectOutput }
	    type { int }
	    access { protected }
	    arglist { "(PortHole* po, GenericPort* source)" }
	    code {
		GenericPort *gp;
		const char* initDelayVals;
		PortHole* farside = po->far();
		if (!farside) {
		  Error::abortRun(*this,"Star is not fully connected");
		  return 0;
		}
		int numdelays = po->numInitDelays();
		initDelayVals = po->initDelayValues();
		farside->disconnect();

		source->connect(*farside,numdelays,initDelayVals);
		if(parent()->isA("InterpGalaxy") && 
		   ((numdelays > 0) || (initDelayVals && *initDelayVals))) {
		  ((InterpGalaxy*)parent())->registerInit("C",
							  source->parent()->name(),
							  source->name(),
							  initDelayVals,
							  farside->parent()->name(),
							  farside->name());
		}
		// Fix aliases if any
		if ((gp = po->aliasFrom()) != 0) {
		    gp->setAlias(*source);
		}
		return 1;
	    }
	}
	method {
	    name { setParams }
	    type { int }
	    access { protected }
	    arglist { "(Block* block, int instanceno)" }
	    code {
		// Set the specified parameter values.
		for (int i = 0; i < parameter_map.size()-1; i++) {

		    // First step is to determine whether the parameter
		    // name is of the form xxx(n) or xxx(n)=, in which case, it only
		    // applies to this instance if n == instanceno.
		    // Use a regular express to parse the form xxx(n).

		    const char* parameter_name = parameter_map[i++];
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
				Tcl_GetVar(ptkInterp,command,NULL);
			Tcl_GetInt(ptkInterp, instance_index, &givenInstNo);

			// If the instance numbers don't match,
			// there is nothing more to do.
			if (!(instanceno == givenInstNo)) continue;

			// If the instance numbers match, then reset
			// parameter_name and proceed with substitutions
			command = "parameter_name";
			parameter_name = Tcl_GetVar(ptkInterp,command,NULL);
			if (!parameter_name) {
			    Error::abortRun(*this,
				"Unxpected error setting parameter_name!");
			    return 0;
			}
			
		    }
		    // Check for the optional "=".  Ignore it.
		    if (strcmp(parameter_map[i],"=") == 0) {
		      if(i < parameter_map.size()-1) i++;
		    }

		    // Whether the regular expression matched or not,
		    // replace references to instance number in the
		    // parameter value. Use Tcl to do this, since it has
		    // such convenient string manipulation functions.

		    command = "regsub instance_number {";
		    command += parameter_map[i];
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
		    	   hashstring(Tcl_GetVar(ptkInterp,command,NULL)))) {
			Error::abortRun(*this,
			    "Bad name in parameter map: ", parameter_name);
			return 0;
		    }
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
	    while (p = bpi++) {
	      PortHole *far = p->far();
	      if (!far) {
		// This might be a galaxy porthole, which should return something,
		// but doesn't.  Find the corresponding real porthole.
		GenericPort &gp = p->realPort();
		if (gp.isA("PortHole")) p = (PortHole*)(&gp);
		far = p->far();
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
	    if (mph = ph->getMyMultiPortHole()) {
	      gp = mph;
	    } else {
	      gp = ph;
	    }
	    // Get the top-level port that we are connected to
	    while (gpt = gp->aliasFrom()) {
	      gp = gpt;
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
