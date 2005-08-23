defstar {
	name { MapGr }
	domain { HOF }
	derivedFrom { Map }
	version { @(#)HOFMapGr.pl	1.15 11/13/97 }
	author { Edward A. Lee, Tom Lane }
	location { HOF main library }
	copyright {
Copyright (c) 1994-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	desc {
A variant of the Map star where the replacement block is specified
by graphically connecting it.
There must be exactly one block connected in the position of the
replacement block.
The HOFNop stars are the only exception: they may be used in addition to the
one replacement block in order to control the order of connection.
	}
	htmldoc {
See the documentation for the <tt>Map</tt>
star, from which this is derived, for background information.
The parameter values of the graphically connected replacement block
serve as default parameter values for all replacement blocks.
These values can be overridden with the <i>parameter_map</i> parameter,
where the format is the same as in other higher-order stars.
        }

	inmulti {
	  name {exin}
	  type {anytype}
	  desc {
Example of a connection to the replacement block
          }
	}
	outmulti {
	  name {exout}
	  type {anytype}
	  desc {
Example of a connection to the replacement block
          }
	}

	ccinclude { "Galaxy.h" }
	ccinclude { "InfString.h" }

	constructor {
	  input_map.clearAttributes(A_SETTABLE);
	  output_map.clearAttributes(A_SETTABLE);
	  blockname.clearAttributes(A_SETTABLE);
	  where_defined.clearAttributes(A_SETTABLE);
	  parameter_map.setInitValue("");
	}

	method {
	  name { doExpansion }
	  type { int }
	  code {
	    // Make sure we know the number of connections on the
	    // input and output multiPortHoles.
	    if (! initConnections(input)) return 0;
	    if (! initConnections(output)) return 0;
	    if (! initConnections(exout)) return 0;
	    if (! initConnections(exin)) return 0;

	    // At this point, any HOFNop stars will have been disconnected,
	    // so there should be only one example star.

	    MPHIter nexti(input);
	    MPHIter nexto(output);
	    MPHIter nextexo(exout);
	    MPHIter nextexi(exin);

	    // Make the first block connection, and in the process,
	    // build the input_map and output_map.
	    // CAUTION: StringArrayState treats '#' as a comment introducer.
	    // To avoid failing with port names like 'input#1', we put
	    // quote marks around them.  A port name containing a quote will
	    // still cause havoc...
	    PortHole *pi, *po, *pei, *peo;
	    InfString inputMap, outputMap;
	    GenericPort *sourcegp, *destgp;
	    myblock = 0;

	    // Iterate over the inputs first
	    while ((peo = nextexo++) != 0) {
	      if ((pi = nexti++) == 0) {
		Error::abortRun(*this,
				"Not enough inputs for the specified replacement block");
		return 0;
	      }
	      if (!(destgp = breakConnection(peo))) return 0;
	      inputMap += " \"";
	      inputMap += destgp->name();
	      inputMap += "\"";
	      if (!connectInput(pi,destgp)) return 0;
	    }
	    input_map.setCurrentValue((const char*)inputMap);

	    // Iterate over the outputs next
	    while ((pei = nextexi++) != 0) {
	      if ((po = nexto++) == 0) {
		Error::abortRun(*this,
				"Not enough outputs for the specified replacement block");
		return 0;
	      }
	      if (!(sourcegp = breakConnection(pei))) return 0;
	      outputMap += " \"";
	      outputMap += sourcegp->name();
	      outputMap += "\"";
	      if (!connectOutput(po, sourcegp)) return 0;
	    }
	    output_map.setCurrentValue((const char*)outputMap);

	    if (!myblock) {
	      Error::abortRun(*this,
			      "No connections to a replacement block!");
	      return 0;
	    }
	    if (myblock->parent() != idParent()) {
	      Error::abortRun(*this,
			      "Replacement block is in wrong galaxy");
	      return 0;
	    }

	    // Iterate over the remaining blocks.
	    if (!iterateOverPorts(nexti,nexto,2)) return 0;

	    // Set the original block's parameter values only after
	    // all the clones have been created,
	    // so that cloning takes the right default values.
	    if (!setParams(myblock, 1)) return 0;

	    return 1;
	  }
	}

	// Override the default method to create new block.
	// Note we do not use the blockname parameter;
	// so it is not necessary to set the blockname state above.
	method {
	    name { createBlock }
	    type { "Block*" }
	    access { protected }
	    arglist { "(Galaxy& mom, const char* /*blockname*/, const char* /*where_defined*/)" }
	    code {
              Block *block = myblock->clone();
	      if (!block) {
		Error::abortRun(*this,
				"Unable to create an instance of block: ",
				myblock->name());
		return NULL;
	      }
	      // Set the target in the new block.
	      // This may no longer be necessary, but it can't hurt.
	      if (target()) block->setTarget(target());

	      // Add to parent galaxy.  Note that since new blocks are
	      // added at the end of the block list, this block will get
	      // a preinitialize call later on.
	      mom.addBlock(*block, genUniqueName(mom, myblock->name()));

	      return block;
            }
	}
}
