static const char file_id[] = "DDFSimpleSched.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "DDFSimpleSched.h"
#include "Target.h"
#include "SDFScheduler.h"
#include "pt_fstream.h"
#include "EventHorizon.h"
#include <std.h>
	
/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the above
copyright notice and the following two paragraphs appear in all copies
of this software.

IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY 
FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES 
ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF 
THE UNIVERSITY OF CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF 
SUCH DAMAGE.

THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND THE UNIVERSITY OF
CALIFORNIA HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
ENHANCEMENTS, OR MODIFICATIONS.
							COPYRIGHTENDKEY

 Programmer:  E. A. Lee
 (Based on DDFClustSched, by Soonhoi Ha)

 This is a simple DDF scheduler where there is no clustering, and
 a basic iteration consists of as many firings as possible, but no
 more than one per actor.  An iteration can consist of more than one
 basic iteration if a pragma is set indicating that a particular star
 should fire some number of times to define an iteration.
 This is meant to replace DynDFScheduler, but for now, inherits
 from it.


**************************************************************************/

///////////////////////////////////////////////////////////////////////
// Check to see whether the specified star has a pragma registered
// with it.  As a side effect, the value of the pragma will be
// parsed and stored in the star.
// In addition, a pointer to the star will be added to the
// pragmaStars list.
//
int DDFSimpleSched :: pragmaRegistered(DataFlowStar* st) {
  // For efficiency, check to see whether st->getIter() returns
  // non-zero.  This means we have already parsed a pragma.
  if (st->getIter() > 0) return 1;

  // The following conditional is a hopefully unnecessary precaution,
  // since the star should certainly have a target.
  if (st->target()) {
    // The pragma is registered under the parent classname
    // and the name of the star.
    StringList pragma = st->target()->pragma(st->parent()->className(),
					     st->name(),
					     "firingsPerIteration");
    if (pragma.length() != 0) {
      // Store the required number of iterations of the star.
      // Note that we will register a zero if no integer is recognized
      // in the specified string.  If the number of iterations is non-zero,
      // then add the star to the pragmaStars list.
      st->setIter(atoi((const char*)pragma));
      if (st->getIter()) pragmaStars.append(st);
      return 1;
    } else {
      st->setIter(0);
    }
  }
  return 0;
}

////////////////////////////////////////////////////////////////////////
// Determine in which class the star passed in falls.
// The possible classes are not-enabled (return 0),
// enabled, non-deferrable (add to enabledNonDef list and return 1),
// enabled, deferrable, non-sources (add to enabledDefNonSources list
// and return 2), enabled, deferrable sources (add to defSources list
// and return 3).
//
int DDFSimpleSched :: classify (DataFlowStar* c) {
  int state = enabledState(c);
  switch(state) {
  case 1:		// enabled and non-deferrable
    enabledNonDef.append(c);
    break;
  case 2:		// enabled, deferrable, non-source
    enabledDefNonSources.append(c);
    break;
  case 3:		// enabled, deferrable sources
    defSources.append(c);
    break;
  }
  return state;
}

////////////////////////////////////////////////////////////////////////
//
void DDFSimpleSched :: setup () {
  DDFScheduler::setup();

  // Check stars for registered pragmas and record in the pragmaStars list.
  pragmaStars.initialize();
  DFGalStarIter nextStar(*(galaxy()));
  DataFlowStar* c;
  while((c = nextStar++) != 0) pragmaRegistered(c);
}

////////////////////////////////////////////////////////////////////////
// The strategy of the runtime DDF scheduler is as follows:
//
// One iteration, by default, consists of firing all enabled
// and non-deferrable actors once. A deferrable actor is one with
// any output arc that has enough data to satisfy the destination actor.
//
// E = enabled actors
// D = deferrable enabled actors
// S = source actors
// F = actors that have fired once already in this one iteration
//
// Note that S is a subset of E.
//
// One default iteration consists of:
//      
//      if (E-D != 0) fire(E-D)
//      else if (D-S != 0) fire (D-S)
//      else if (S != 0) fire (S)
//      else deadlocked.
// 
// This default iteration is defined to fire actors
// at most once.  A larger notion of an iteration can be specified using
// the targetPragmas mechanism to identify particular stars that must fire
// some specific number of times (greater than or equal to one) in
// each iteration.  This will be accomplished by executing enough default
// iterations until these specified numbers are met or exceeded.
//
int DDFSimpleSched :: run() {
  if (!galaxy()) {
    Error::abortRun("No galaxy to run");
    return FALSE;
  }
  if (haltRequested()) return FALSE;

  DFGalStarIter nextStar(*(galaxy()));
  DataFlowStar* c;

  while (numFiring < stopTime && !haltRequested()) {

    // Reset the firings count for all stars.
    // This is used (optionally) to determine how many star
    // firings contitute one "iteration".  To come into play,
    // you must set the firingsPerIteration pragma of the star.
    nextStar.reset();
    while ((c = nextStar++) != 0) c->resetFirings();

    // The following loop might be repeated if any of the stars in
    // it return zero in the enoughFirings() method.
    int doAgain = TRUE;
    while (doAgain) {
      doAgain = FALSE;

      // Classify all stars.  In principle, we could keep a running track
      // of which stars have their enabled state changed by firings,
      // but requires fairly complex data structures, and is unlikely
      // to be faster than this simple method.
      enabledDefNonSources.initialize();
      defSources.initialize();
      nextStar.reset();
      while((c = nextStar++) != 0) classify(c);

      // (1) Fire all enabled and non-deferrable actors at most once.
      int firedOne = FALSE;
      while (enabledNonDef.size() > 0) {
	DataFlowStar *s = (DataFlowStar*)enabledNonDef.getAndRemove();
	if (haltRequested() || !s->run()) return FALSE;
	firedOne = TRUE;
      }

      // (2) If the above yielded no firings at all, then we proceed to the
      //     step of firing all enabled deferrable actors that are not sources.
      if (!firedOne) {
	while (enabledDefNonSources.size() > 0) {
	  DataFlowStar *s = (DataFlowStar*)enabledDefNonSources.getAndRemove();
	  if (haltRequested() || !s->run()) return FALSE;
	  firedOne = TRUE;
	}
      }

      // (3) If the above still yielded no firings, then the graph
      //     cannot be executed in bounded memory.  We now simply fire
      //     all sources, all of which must be deferrable, and which
      //     together comprise all enabled actors.
      if (!firedOne) {
	while (defSources.size() > 0) {
	  DataFlowStar *s = (DataFlowStar*)defSources.getAndRemove();
	  if (haltRequested() || !s->run()) return FALSE;
	  firedOne = TRUE;
	}
      }
	  
      // (4) If we still have not fired a star, we are deadlocked.
      if (!firedOne) {
	if (numFiring < stopTime && !runUntilDeadlock) {
	  Error :: abortRun("deadlock detected: check for  ",
			    "insufficient initial tokens on feedback arcs");
	  return FALSE;
	}
      }

      // Check to see whether enough firings have occurred to satisfy
      // all registered "firingsPerIteration" pragmas.
      ListIter ns(pragmaStars);
      DataFlowStar* np;
      while ((np = (DataFlowStar*)ns++) != 0) {
	if (!np->enoughFirings()) {
	  doAgain = TRUE;
	  break;
	}
      }

      if (runUntilDeadlock) doAgain = firedOne;
    }

    // end of ONE iteration
    numFiring++;	

    // Check for buffer overflow.
    // FIXME: This should be optional, for efficiency.
    nextStar.reset();
    while ((c = nextStar++) != 0) {
      DFStarPortIter nextPort(*c);
      DFPortHole *p = 0;
      while ((p = nextPort++) != 0) {
	if (p->isItInput()) {
	  if (p->numTokens() > maxToken) {
	    reportArcOverflow(p, maxToken);
	    return FALSE;
	  }
	}
      }
    }
  }
  return TRUE;
}

///////////////////////////////////////////////////////////////////////
// Return TRUE if the star is a source (has no inputs).
//
int DDFSimpleSched :: isSource(Star& s) {
  BlockPortIter nextp(s);
  PortHole* p;
  while ((p = nextp++) != 0) {
    if (p->isItInput()) return FALSE;
  }
  return TRUE;
}
///////////////////////////////////////////////////////////////////////
// Check the input ports and return the state of the actor.
// If it is enabled and non-deferrable, return 1.
// If it is a non-source, enabled and deferrable, return 2.
// If it is a deferrable source, return 3.
// Otherwise, return 0 (it is not enabled).
// An actor is deferrable if all its output connections already
// have enough data to enable the downstream actor.
//
int DDFSimpleSched :: enabledState(DataFlowStar* s) {

  // Check to see whether the star has a waitPort (a port it is
  // waiting for data on), and whether that port has enough data.

  DFPortHole* wp = s->waitPort();
  if (wp) {
    // If fewer tokens are available than are needed, not enabled.
    if (wp->numTokens() < s->waitTokens()) return 0;

    // If there are enough inputs, and the output ports do not have
    // enough data already, enabled and not deferrable.
    if (!isOutputDeferrable(s)) return 1;

    // The actor is enabled and deferrable.  It cannot possibly be
    // a source, since it has a wait port.
    return 2;
  }

  // If s has no waitPort, then check all inputs
  // for enough data, and check outputs for enough data.

  if (blockedOnInput(s) > 0) return 0;         // not enabled
  // enabled
  else if (!isOutputDeferrable(s)) return 1;   // enabled, not deferrable
  // enabled and deferrable
  else if (!isSource(*s)) return 2;            // enabled, deferrable, non-source
  // enabled, deferrable source
  else return 3;                               // enabled, deferrable, source
}

///////////////////////////////////////////////////////////////////////
// Check to see whether a star is blocked on input.
// Return 1 if it is, 0 otherwise.  So a return value of zero
// means that the star is enabled.
//
int DDFSimpleSched :: blockedOnInput(DataFlowStar* s) {
  DFStarPortIter nextPort(*s);
  DFPortHole *p;
  while ((p = nextPort++) != 0) {
    if (p->isItInput()) {
      // If the arc is over the limit, flag it.
      if (p->numTokens() > maxToken) overFlowArc = p;

      if (p->numTokens() < p->numXfer()) return 1;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////
// Check to see whether any of the outputs has enough data already to
// satisfy the downstream actor.  If there are no outputs, always return FALSE.
//
int DDFSimpleSched :: isOutputDeferrable(DataFlowStar* s) {
  DFStarPortIter nextPort(*s);
  DFPortHole *p;
  while ((p = nextPort++) != 0) {
    if (p->isItInput()) continue;

    // If output is not connected, continue
    if (!p->far()) continue;

    DataFlowStar* ds = (DataFlowStar*) p->far()->parent();
    DFPortHole* wp = ds->waitPort();
    if (wp && (p->far() == (PortHole*) wp)) {
      // The downstream actor is waiting for data on this port.
      // If the number of tokens available is sufficient...
      if (wp->numTokens() >= ds->waitTokens()) return 1;
    } else if (wp) {
      // The downstream actor is waiting for data
      // on another port.
      return 1;
    } else {
      // The downstream actor is waiting for data on this
      // and other ports (probably an SDF actor).
      if (p->far()->numXfer() <= p->numTokens()) return 1;
    }
  }
  return 0;
}
		
