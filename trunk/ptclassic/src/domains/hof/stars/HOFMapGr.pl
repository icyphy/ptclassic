defstar {
	name { MapGr }
	domain { HOF }
	derivedFrom { Map }
	desc {
A variant of the Map star where the replacement block is specified
by graphically connecting it.  There must be exactly one block
connected in the position of the replacement block.  The HOFNop
stars are the only exception: they may be used in addition to the
one replacement block in order to control the order of connection.
	}
	explanation {
See the documentation for the
.c Map
star, from which this is derived, for background information.
The parameter values of the graphically connected replacement block
serve as default parameter values for all replacement blocks.
These values will be overridden with the \fIparameter_map\fR parameter,
where the format is the same as in other higher-order stars.
        }
	version { $Id$ }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { HOF main library }
	ccinclude { "ptk.h" }
	ccinclude { "SimControl.h" }
	ccinclude { "InfString.h" }
	outmulti {
	  name {exout}
	  type {=input}
	  desc {
Example of a connection to the replacement block
          }
	}
	inmulti {
	  name {exin}
	  type {ANYTYPE}
	  desc {
Example of a connection to the replacement block
          }
	}
	constructor {
	  output.inheritTypeFrom(exin);
	  input_map.clearAttributes(A_SETTABLE);
	  output_map.clearAttributes(A_SETTABLE);
	  blockname.clearAttributes(A_SETTABLE);
	  where_defined.clearAttributes(A_SETTABLE);
	  parameter_map.setInitValue("");
	}
	setup {
	  HOFBaseHiOrdFn::setup();

	  Galaxy* mom = idParent();
	  if(!mom) return;

	  // Any HOF star has to call this method for all multiPortHoles
	  // to be sure that HOFNop stars are dealt with properly.
	  initConnections(exout);
	  initConnections(exin);
	  initConnections(input);
	  initConnections(output);

	  // At this point, any HOFNop stars will have been disconnected,
	  // so there should be only one example star.

	  MPHIter nexti(input);
	  MPHIter nexto(output);
	  MPHIter nextexo(exout);
	  MPHIter nextexi(exin);

	  // Make the first block connection, and in the process, and set
	  // the input_map and output_map and blockname states.
	  PortHole *pi, *po, *pei, *peo;
	  InfString inputMap, outputMap;
	  GenericPort *sourcegp, *destgp;
	  myblock = 0;

	  // Iterate over the inputs first
	  while ((peo = nextexo++) != 0) {
	    if ((pi = nexti++) == 0) {
	      Error::abortRun(*this,
			      "Not enough inputs for the specified replacement block");
	      return;
	    }
	    if (!(destgp = breakConnection(peo))) return;
	    if (inputMap.numPieces() > 0) inputMap += " ";
	    inputMap += destgp->name();
	    if (!connectInput(pi,destgp)) return;
	  }
	  input_map.setInitValue((const char*)inputMap);
	  input_map.initialize();
	  // Iterate over the outputs next
	  while ((pei = nextexi++) != 0) {
	    if ((po = nexto++) == 0) {
	      Error::abortRun(*this,
			      "Not enough outputs for the specified replacement block");
	      return;
	    }
	    if (!(sourcegp = breakConnection(pei))) return;
	    if (outputMap.numPieces() > 0) outputMap += " ";
	    outputMap += sourcegp->name();
	    if (!connectOutput(po, sourcegp)) return;
	  }
	  if (!myblock) {
	    Error::abortRun(*this,
			    "No connections to a replacement block!");
	    return;
	  }
	  output_map.setInitValue((const char*)outputMap);
	  output_map.initialize();
	  blockname.setInitValue(myblock->name());
	  blockname.initialize();
	  
	  // Iterate over the remaining blocks.
	  if (!iterateOverPorts(nexti,nexto,mom,2)) return;

	  // Set the parameter values after all the clones have been created
	  // so that cloning takes the right default values.
	  if(!setParams(myblock, 1)) return;
	  myblock->setTarget(target());
	  myblock->initialize();

	  mom->deleteBlockAfterInit(*this);
	}
	// Override the default method to create new block.
	method {
	    name { createBlock }
	    type { "Block*" }
	    access { protected }
	    arglist { "(Galaxy& mom, const char* blockname, const char* where_defined)" }
	    code {
              Block *block = myblock->clone();
	      if (!block) {
		Error::abortRun(*this,
				"Unable to create instance of block: ",
				myblock->name());
	      }
	      // Choose a name for the block
	      StringList instancename = "Map_";
	      instancename += (const char*)myblock->name();
	      instancename += "__";
	      instancename += count++;
	      // The following amounts to a small memory leak
	      const char* instance = hashstring(instancename);

	      mom.addBlock(*block,instance);
	      return block;
            }
	}
}
