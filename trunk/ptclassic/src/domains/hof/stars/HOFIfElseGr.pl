defstar {
	name { IfElseGr }
	domain { HOF }
	derivedFrom { IfElse }
	desc {
A variant of the IfElse star where the two possible replacement
blocks are specified graphically rather than textually.
There must be exactly one block connected in the position of each
of the two replacement blocks.
The HOFNop stars are the only exception: they may be used in addition to the
replacement block in order to control the order of connection.
	}
	htmldoc {
See the documentation for the
<tt>IfElse</tt>
star, from which this is derived, for background information.
The parameter values for the replacement blocks are set directly
by editing the parameters of the block.
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
	ccinclude { "InfString.h" }
	outmulti {
	  name {trueout}
	  type {anytype}
	  desc {output to the true block}
	}
	inmulti {
	  name {truein}
	  type {anytype}
	  desc {input from the true block}
	}
	outmulti {
	  name {falseout}
	  type {anytype}
	  desc {output to the false block}
	}
	inmulti {
	  name {falsein}
	  type {anytype}
	  desc {input from the false block}
	}
	constructor {
	  // The constructor for IfElse hides blockname, where_defined,
	  // input_map, output_map, and parameter_map.  Here, we need to hide
	  // the rest.
	  true_block.clearAttributes(A_SETTABLE);
	  where_true_defined.clearAttributes(A_SETTABLE);
	  true_input_map.clearAttributes(A_SETTABLE);
	  true_output_map.clearAttributes(A_SETTABLE);
	  true_parameter_map.clearAttributes(A_SETTABLE);
	  false_block.clearAttributes(A_SETTABLE);
	  where_false_defined.clearAttributes(A_SETTABLE);
	  false_input_map.clearAttributes(A_SETTABLE);
	  false_output_map.clearAttributes(A_SETTABLE);
	  false_parameter_map.clearAttributes(A_SETTABLE);
	}
	protected {
	  Block *trueblock, *falseblock;
	}
	method {
	  name { preinitialize }
	  access { public }
	  code {
	    HOFBaseHiOrdFn::preinitialize();

	    Galaxy* mom = idParent();
	    if(!mom) return;

	    // Make sure we know the number of connections on the
	    // input and output multiPortHoles.
	    initConnections(input);
	    initConnections(output);
	    initConnections(trueout);
	    initConnections(truein);
	    initConnections(falseout);
	    initConnections(falsein);

	    // At this point, any HOFNop stars will have been disconnected,
	    // so there should be only one block connected to each of the
	    // true and false connections.

	    MPHIter nexti(input);
	    MPHIter nexto(output);
	    MPHIter *nextexo;
	    MPHIter *nextexi;
	    MPHIter *nextnuo;
	    MPHIter *nextnui;
	    // Choose the porthole over which to iterate
	    if (int(condition)) {
	      // Iterators for ports connected to the block we will use
	      LOG_NEW; nextexo = new MPHIter(trueout);
	      LOG_NEW; nextexi = new MPHIter(truein);
	      // Iterators for ports connected to the block we won't use
	      LOG_NEW; nextnuo = new MPHIter(falseout);
	      LOG_NEW; nextnui = new MPHIter(falsein);
	    } else {
	      LOG_NEW; nextexo = new MPHIter(falseout);
	      LOG_NEW; nextexi = new MPHIter(falsein);
	      LOG_NEW; nextnuo = new MPHIter(trueout);
	      LOG_NEW; nextnui = new MPHIter(truein);
	    }

	    PortHole *pi, *po, *pei, *peo;
	    GenericPort *sourcegp, *destgp;

	    myblock = 0;	// this will get a ref to the replacement block

	    // Make the block connection
	    // Iterate over the inputs first
	    while ((peo = (*nextexo)++) != 0) {
	      if ((pi = nexti++) == 0) {
		Error::abortRun(*this,
				"Not enough inputs for the specified replacement block");
		return;
	      }
	      if (!(destgp = breakConnection(peo))) return;
	      if (!connectInput(pi,destgp)) return;
	    }
	    // Better be out of outputs at this point
	    if ((pi = nexti++) != 0) {
	      Error::abortRun(*this, "Leftover inputs after connecting the replacement block");
	      return;
	    }
	    // Iterate over the outputs next
	    while ((pei = (*nextexi)++) != 0) {
	      if ((po = nexto++) == 0) {
		Error::abortRun(*this,
				"Not enough outputs for the specified replacement block");
		return;
	      }
	      if (!(sourcegp = breakConnection(pei))) return;
	      if (!connectOutput(po, sourcegp)) return;
	    }
	    if (!myblock) {
	      Error::abortRun(*this,
			      "No connections to replacement block!");
	      return;
	    }

	    myblock = 0;	// now get a reference to the unused block

	    // Disconnect the block that we will not use
	    while ((peo = (*nextnuo)++) != 0) {
	      breakConnection(peo);
	    }
	    while ((pei = (*nextnui)++) != 0) {
	      breakConnection(pei);
	    }
	    if (!myblock) {
	      Error::abortRun(*this,
			      "No connections to unused replacement block!");
	      return;
	    }

	    LOG_DEL; delete nextexo;
	    LOG_DEL; delete nextexi;
	    LOG_DEL; delete nextnuo;
	    LOG_DEL; delete nextnui;

	    // Delete the unused block from the parent galaxy.
	    // This is *critical* to stop a graphical recursion.
	    mom->deleteBlockAfterInit(*myblock);

	    mom->deleteBlockAfterInit(*this);
	  }
	}
}
