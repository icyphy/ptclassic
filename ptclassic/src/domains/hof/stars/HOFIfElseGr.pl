defstar {
	name { IfElseGr }
	domain { HOF }
	derivedFrom { IfElse }
	version { @(#)HOFIfElseGr.pl	1.13 11/13/97 }
	author { Edward A. Lee, Tom Lane }
	location { HOF main library }
	copyright {
Copyright (c) 1994-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
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
by editing the parameters of the blocks.
        }

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

	ccinclude { "Galaxy.h" }

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

	method {
	  name { doExpansion }
	  type { int }
	  code {
	    // Make sure we know the number of connections on the
	    // input and output multiPortHoles.
	    if (! initConnections(input)) return 0;
	    if (! initConnections(output)) return 0;
	    if (! initConnections(truein)) return 0;
	    if (! initConnections(trueout)) return 0;
	    if (! initConnections(falsein)) return 0;
	    if (! initConnections(falseout)) return 0;

	    // At this point, any HOFNop stars will have been disconnected,
	    // so there should be only one block connected to each of the
	    // true and false connections.

	    MPHIter nexti(input);
	    MPHIter nexto(output);
	    MPHIter nexttruein(truein);
	    MPHIter nexttrueout(trueout);
	    MPHIter nextfalsein(falsein);
	    MPHIter nextfalseout(falseout);

	    // Choose the porthole over which to iterate
	    MPHIter *nextexo;
	    MPHIter *nextexi;
	    MPHIter *nextnuo;
	    MPHIter *nextnui;
	    if (int(condition)) {
	      // Iterators for ports connected to the block we will use
	      nextexo = &nexttrueout;
	      nextexi = &nexttruein;
	      // Iterators for ports connected to the block we won't use
	      nextnuo = &nextfalseout;
	      nextnui = &nextfalsein;
	    } else {
	      nextexo = &nextfalseout;
	      nextexi = &nextfalsein;
	      nextnuo = &nexttrueout;
	      nextnui = &nexttruein;
	    }

	    Galaxy* mom = idParent();
	    if (!mom) return 0;

	    PortHole *pi, *po, *pei, *peo;
	    GenericPort *sourcegp, *destgp;

	    myblock = 0;	// this will get a ref to the replacement block

	    // Make the block connection
	    // Iterate over the inputs first
	    while ((peo = (*nextexo)++) != 0) {
	      if ((pi = nexti++) == 0) {
		Error::abortRun(*this,
				"Not enough inputs for the specified replacement block");
		return 0;
	      }
	      if (!(destgp = breakConnection(peo))) return 0;
	      if (!connectInput(pi,destgp)) return 0;
	    }
	    // Better be out of inputs at this point
	    if ((pi = nexti++) != 0) {
	      Error::abortRun(*this,
			      "Leftover inputs after connecting the replacement block");
	      return 0;
	    }
	    // Iterate over the outputs next
	    while ((pei = (*nextexi)++) != 0) {
	      if ((po = nexto++) == 0) {
		Error::abortRun(*this,
				"Not enough outputs for the specified replacement block");
		return 0;
	      }
	      if (!(sourcegp = breakConnection(pei))) return 0;
	      if (!connectOutput(po, sourcegp)) return 0;
	    }
	    // Better be out of outputs at this point
	    if ((po = nexto++) != 0) {
	      Error::abortRun(*this,
			      "Leftover outputs after connecting the replacement block");
	      return 0;
	    }

	    if (!myblock) {
	      Error::abortRun(*this,
			      "No connections to replacement block!");
	      return 0;
	    }
	    if (myblock->parent() != mom) {
	      Error::abortRun(*this,
			      "Replacement block is in wrong galaxy");
	      return 0;
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
	      return 0;
	    }
	    if (myblock->parent() != mom) {
	      Error::abortRun(*this,
			      "Unused replacement block is in wrong galaxy");
	      return 0;
	    }

	    // Delete the unused block from the parent galaxy.
	    // This is *critical* to stop a graphical recursion.
	    mom->deleteBlockAfterInit(*myblock);

	    return 1;
	  }
	}
}
