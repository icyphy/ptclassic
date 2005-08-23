defstar {
	name {BaseHiOrdFn}
	domain {HOF}
	derivedFrom {Base}
	version { @(#)HOFBaseHiOrdFn.pl	1.14 11/13/97 }
	author { Edward A. Lee, Tom Lane }
	location { HOF main library }
	copyright {
Copyright (c) 1994-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
A higher-order function is a function that takes a function as
an argument and/or returns a function.
Stars and galaxies in Ptolemy have two kinds of arguments:
signals and parameters.
The higher-order functions supported by this base class
take functions as parameters, not as signals.
The basic mechanism is that a star or galaxy is statically specified
by a parameter, and the higher-order star replaces itself at preinitialization
time with one or more instances of the specified replacement block.
<p>
The name of the replacement block is given by the <i>blockname</i> parameter.
If the replacement block is a galaxy, then the <i>where_defined</i> parameter
gives the full name (including the full path) of the definition of the galaxy.
This path name may (and probably should) begin with the environment
variable $PTOLEMY or ~username.
<p>
Currently, this must be an oct facet, although in the future, other
specifications (like ptcl files) may be allowed.
The oct facet should contain the replacement galaxy,
or it could simply be the facet that defines the replacement galaxy.
If the replacement block is a built-in star, then there
is no need to give a value to the <i>where_defined</i> parameter.
<p>
The stars derived from this base class differ mostly in the way
that replacement blocks get wired into the graph.
They share a common mechanism for specifying the value
of parameters in the replacement block.
<h3>Setting parameter values</h3>
<p>
The <i>parameter_map</i> parameter can be used to set parameter values
for the replacement blocks.
There are four acceptable forms:
<pre>
name value
name(number) value
name = value
name(number) = value
</pre>
There should be no spaces between "name" and "(number)", and the name
cannot contain spaces, "=", or "(".
In all cases, <i>name</i> is the name of a parameter in the
replacement block.
In the first case, the value is applied to all instances
of the replacement block.
In the second case, it is applied only to the specified instance
number (which starts with 1).  The third and fourth cases just
introduce an optional equal sign, for readability.
If the "=" is used, there must be spaces around it.
<p>
The value can be any usual Ptolemy expression for giving the value
of a parameter.
However, if the string "instance_number" appears anywhere in the
expression, it will be replaced with the instance number of the
replacement block.
Note that it need not be a separate token.
For example, the value "xxxinstance_numberyyy" will become "xxx9yyy"
for instance number 9.
After this replacement, the value is evaluated using the usual
Ptolemy expression evaluator for initializing states.
	}

	ccinclude { "Galaxy.h" }
	ccinclude { "KnownBlock.h" }
	ccinclude { "InfString.h" }
	ccinclude { "ptk.h" }
	ccinclude { <string.h> }
	ccinclude { <ctype.h> }

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
	  int nameCount;	// for assigning unique identifiers
	  Block* myblock;	// for checking there is only 1 replacement blk
	}

	constructor {
	  nameCount = 0;
	  myblock = 0;
	}

	virtual method {
	    name { createBlock }
	    type { "Block*" }
	    access { protected }
	    arglist { "(Galaxy& mom, const char* blockname, const char* where_defined)" }
	    code {
	        // Create the star and install in the galaxy
	        if (!KnownBlock::find(blockname, mom.domain()) &&
		    where_defined && *where_defined) {

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

		// Add to parent galaxy.  Note that since new blocks are
		// added at the end of the block list, this block will get
		// a preinitialize call later on.
		mom.addBlock(*block, genUniqueName(mom, blockname));

		return block;
	    }
	}

	// Choose a name for a new block that's unique within its galaxy.
	method {
	    name { genUniqueName }
	    type { "const char *" }
	    access { protected }
	    arglist { "(Galaxy& mom, const char* blockname)" }
	    code {
		// We must hashstring the selected name; that amounts to
		// a small memory leak.  We minimize the leakage by only
		// insisting on local uniqueness, not global uniqueness.
	        StringList instancename;
		do {
		  instancename = blockname;
		  instancename += "_HOF_";
		  instancename += ++nameCount;
		} while (mom.blockWithName(instancename) != NULL);
		const char* instance = hashstring(instancename);
		return instance;
	    }
	}

	// Given a PortHole*, this method disconnects it from
	// whatever it is connected to, and returns the top-level
	// GenericPort* (i.e. galaxy port) for the far side of
	// that connection.  As a side effect, if the protected
	// member "myblock" is NULL, we set it to point to the
	// block on the far side of the connection.  If it is already
	// non-NULL, we check to see that the block on the far side is
	// exactly the same as the one pointed to by "myblock".
	// If any error occurs, NULL is returned.
	// The primary reason for returning the top alias, and not just
	// the connected-to port, is that the connected-to port may be
	// inside a subgalaxy, and we want "myblock" to refer to the
	// whole subgalaxy in that case.
	method {
	  name { breakConnection }
	  type { "GenericPort*" }
	  access { protected }
	  arglist { "(PortHole* peo)" }
	  code {
	    PortHole* dest = peo->far();
	    if (dest == 0) {
	      Error::abortRun(*this,
			      "Star is not fully connected");
	      return NULL;
	    }
	    GenericPort *destgp = &(dest->getTopAlias());
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
	    // Any delay on the example connection will be ignored;
	    // but warn the user about it.
	    int numDelays = dest->numInitDelays();
	    const char* delayVals = dest->initDelayValues();
	    if (numDelays > 0 || (delayVals && *delayVals)) {
	      Error::warn(*this,
			  "Ignoring delay between HOF star and its "
			  "replacement block. Put the delay on the other side "
			  "of the HOF star.");
	    }
	    dest->disconnect();
	    return destgp;
	  }
	}

	// Find the named porthole of the given block; return NULL if bad name.
	// This is almost just Block::genPortWithName, except that we have
	// to be prepared to instantiate more members of a multiport if the
	// given name is of the form "multiportname#n".
	static method {
	  name { findPortWithName }
	  type { "GenericPort *" }
	  access { protected }
	  arglist { "(Block *block, const char* name)" }
	  code {
	    // Easy if the port already exists.
	    GenericPort *gp = block->genPortWithName(name);
	    if (gp) return gp;
	    // See if name is parsable as an MPH name + number.
	    const char* hashloc = strrchr(name, '#');
	    if (!hashloc || !isdigit(hashloc[1])) return 0;
	    int portIndex = atoi(hashloc+1);
	    if (portIndex <= 0) return 0;
	    char* mphname = savestring(name);
	    mphname[hashloc-name] = '\0';
	    MultiPortHole *mph = block->multiPortWithName(mphname);
	    delete [] mphname;
	    if (!mph) return 0;
	    // OK, make the right number of members.
	    for (int i = mph->numberPorts(); i < portIndex; i++)
	      mph->newPort();
	    // Now it should work.
	    return block->genPortWithName(name);
	  }
	}

	// Verify that all the portholes of block are connected to star.
	// (Hidden portholes are excepted.)
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

	method {
	    name { setParams }
	    type { int }
	    access { protected }
	    arglist { "(Block* block, int instanceno)" }
	    code {
		// Set the specified parameter values.
		for (int i = 0; i < parameter_map.size()-1; i++) {

		    const char* parameter_name = parameter_map[i++];

		    // Check for the optional "=".  Ignore it.
		    if (strcmp(parameter_map[i],"=") == 0) {
		      if(i < parameter_map.size()-1) i++;
		    }

		    if (!setParameter(block, instanceno, parameter_name,
				      (const char*)parameter_map[i])) {
		      return 0;
		    }
		}
		return 1;
	    }
	}
}
