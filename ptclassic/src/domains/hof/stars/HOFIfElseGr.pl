defstar {
	name { IfElseGr }
	domain { HOF }
	derivedFrom { IfElse }
	desc {
A variant of the IfElse star where the two possible replacement
blocks are specified by graphically rather than textually.  There must
be exactly two blocks connected in the positions of the replacement blocks.
The HOFNop stars are the only exception: they may be used in addition to the
two replacement blocks in order to control the order of connection.
	}
	explanation {
See the documentation for the
.c IfElse
star, from which this is derived, for background information.
The parameter values for the replacement blocks are set directly
by editing the parameters of the block.
        }
	version { $Id$ }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { HOF main library }
	ccinclude { "ptk.h" }
	ccinclude { "SimControl.h" }
	ccinclude { "InfString.h" }
	ccinclude { "HOFDelayedMap.h" }
	outmulti {
	  name {trueout}
	  type {=input}
	  desc {output to the true block}
	}
	inmulti {
	  name {truein}
	  type {ANYTYPE}
	  desc {input from the true block}
	}
	outmulti {
	  name {falseout}
	  type {=input}
	  desc {output to the false block}
	}
	inmulti {
	  name {falsein}
	  type {ANYTYPE}
	  desc {input from the false block}
	}
	constructor {
	  // Note that both branches must have the same output type.
	  output.inheritTypeFrom(truein);

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
	setup {
	  HOFBaseHiOrdFn::setup();

	  Galaxy* mom = idParent();
	  if(!mom) return;

	  // Any HOF star has to call this method for all multiPortHoles
	  // to be sure that HOFNop stars are dealt with properly.
	  initConnections(trueout);
	  initConnections(truein);
	  initConnections(falseout);
	  initConnections(falsein);
	  initConnections(input);
	  initConnections(output);

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

	  PortHole *pi, *po, *pei, *peo, *dest;
	  GenericPort *sourcegp, *destgp;

	  // Now that we know which substitution block will be used, it
	  // is safe to initialize that substitution block even if it is
	  // a recursive reference to a galaxy above us.
	  // This is a violation of information hiding, but I can't figure
	  // out a cleaner way to support graphical recursion.
	  if ((peo = (*nextexo)++) &&
	      (dest = peo->far()) &&
	      (myblock = dest->parent()) &&
	      (myblock->isA("HOFDelayedMap"))) {
	    destgp = findTopGenericPort(dest);
	    destgp->parent()->setTarget(target());
	    destgp->parent()->initialize();
	    ((HOFDelayedMap*)myblock)->substitute();
	  }
	  myblock = 0;
	  nextexo->reset();

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
			    "No connections to a replacement block!");
	    return;
	  }

	  myblock->setTarget(target());
	  myblock->initialize();
	  myblock = 0;

	  // Disconnect the block that we will not use
	  while ((peo = (*nextnuo)++) != 0) {
	    breakConnection(peo);
          }
	  while ((pei = (*nextnui)++) != 0) {
	    breakConnection(pei);
          }

	  LOG_DEL; delete nextexo;
	  LOG_DEL; delete nextexi;
	  LOG_DEL; delete nextnuo;
	  LOG_DEL; delete nextnui;

	  // delete the unused block.
	  mom->deleteBlockAfterInit(*myblock);

	  mom->deleteBlockAfterInit(*this);
	}
}
