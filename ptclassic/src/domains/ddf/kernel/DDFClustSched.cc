static const char file_id[] = "DDFClustSched.cc";
#ifdef __GNUG__
#pragma implementation
#endif

#include "type.h"
#include "DDFClustSched.h"
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

 Programmers:  Soonhoi Ha, J. Buck, E. A. Lee

The basic dynamic scheduler is the class DynDFScheduler.
This derived class adds restructuring code, which clusters the graph
into SDF clusters.

**************************************************************************/
	
///////////////////////////////////////////////////////////////////////
// After compile-time scheduling, change the numTokens value
// of the EventHorizons.  This applies only to wormholes.
// Note that this is an ordinary function, not a method.
//
static void renewNumTokens (Block* b, int flag) {
 
  BlockPortIter nextp(*b);
  PortHole* p;
  
  while ((p = nextp++) != 0) {
    EventHorizon* eveP = p->asEH();
    PortHole* ghostP = eveP->ghostAsPort();
    PortHole* bP = ghostP->far();
    int num = bP->numXfer();
    DataFlowStar* s = (DataFlowStar*) bP->parent();
    if (flag == TRUE) {
      num = num * s->repetitions.num();
    } else {
      if (num == 0) num = 1;
    }
    ghostP->asEH()->setParams(num);
  }
}

///////////////////////////////////////////////////////////////////////
// setup
//
void DDFClustSched :: setup () {
  if (logFile && *logFile) {
    logstrm = new pt_ofstream(logFile);
    if (!*logstrm) {
      LOG_DEL; delete logstrm;
      return;
    }
  }

  // If the entire galaxy is SDF, do SDFScheduling.
  if (amISDF) {
    if (!galaxy()) {
      Error::abortRun("DDFClustSched: no galaxy!");
      return;
    }
    clearHalt();
    realSched->setup();
    return;
  }
  DynDFScheduler::setup();
}

///////////////////////////////////////////////////////////////////////
// Create SDF clusters.
//
void DDFClustSched::initStructures() {

  if (restructured == FALSE) {
    LOG_DEL; delete cgal;

    // create a flat galaxy with each star being one cluster.
    LOG_NEW; cgal = new SDFClusterGal(*galaxy(), logstrm);

    // cluster the stars into SDF galaxies.
    makeSDFClusters(*cgal);

    selectScheduler(*cgal);
    restructured = TRUE;
  }

  Block* gpar = galaxy()->parent();
  if (gpar && gpar->isItWormhole()) 
    renewNumTokens(gpar, FALSE);
}

///////////////////////////////////////////////////////////////////////
// selectScheduler.
//
void DDFClustSched :: selectScheduler(SDFClusterGal& cgalaxy) {

  // check whether the galaxy is SDF or not
  if (amISDF) {
    LOG_NEW; realSched = new SDFClustSched;
    realSched->setGalaxy(*galaxy());
    realSched->setup();
    Block* gpar = galaxy()->parent();
    if (gpar && gpar->isItWormhole()) 
      renewNumTokens(gpar, TRUE);
    return;
  }

  // The top-level is DDF. Schedule the SDF clusters.
  cgalaxy.genSubScheds();

  // set the sample rates of cluster portholes
  SDFClusterGalIter nextClust(cgalaxy);
  SDFCluster* c;
  while ((c = nextClust++) != 0) {
    if (!c->asBag()) continue;
    // Now, it is an SDFCluster. set the sample rates.
    SDFClustPortIter nextPort(*c);
    SDFClustPort *p;
    while ((p = nextPort++) != 0) {
      int i = p->numIO() * p->inPtr()->parentClust()->reps();
      p->setNumXfer(i);
    }
  }

  setGalaxy(cgalaxy);
}

///////////////////////////////////////////////////////////////////////
// constructor: set default options
// By default, no restructuring is done, and a DDF scheduler is always used.
// To enable restrucuturing, you must set the appropriate target parameter.
//
DDFClustSched::DDFClustSched (const char* log):
	logFile(log), logstrm(0), realSched(0), cgal(0), runUntilDeadlock(0) {
  amISDF = FALSE;
  restructured = FALSE;
}

///////////////////////////////////////////////////////////////////////
// destructor
//
DDFClustSched::~DDFClustSched() { 
  LOG_DEL; delete realSched; 
  LOG_DEL; delete logstrm;
  LOG_DEL; delete cgal;
}

///////////////////////////////////////////////////////////////////////
// Check to see whether the star associated with the specified atomic
// cluster has a pragma registered with it.  As a side effect,
// the value of the pragma will be parsed and stored in the star.
// In addition, a pointer to the star will be added to the
// pragmaStars list.
//
int DDFClustSched :: pragmaRegistered(SDFAtomCluster* as) {
  // For efficiency, check to see whether as->getIter() returns
  // non-zero.  This means we have already parsed a pragma.
  if (as->real().getIter() > 0) return 1;

  // The following conditional is a hopefully unnecessary precaution,
  // since the star should certainly have a target.
  if (as->real().target()) {
    // The pragma is registered under the parent classname
    // and the name of the star.
    StringList pragma = as->real().target()->pragma(as->real().parent()->className(),
						    as->real().name(),
						    "firingsPerIteration");
    if (pragma.length() != 0) {
      // Store the required number of iterations of the cluster.
      // Note that this will register a zero if no integer is recognized
      // in the specified string.
      as->real().setIter(atoi((const char*)pragma));
      pragmaStars.append(&(as->real()));
      return 1;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////
// Group SDF stars as a cluster.
// This grouping will hopefully improve the runtime speed by using the
// compile-time SDF scheduler.  If a star has been annotated with the
// firingsPerIteration target pragma, then do not cluster it.
// The considerable overhead of having a parallel tree, however, makes
// it doubtful that this is really faster than straight DDF scheduling,
// except for very long runs.
//
int DDFClustSched :: makeSDFClusters (SDFClusterGal& galaxy) {

  if (logstrm) {
    *logstrm << " clustering SDF stars: begin...\n";
  }

  SDFClusterGalIter nextClust(galaxy);

  amISDF = FALSE;
  int flagDDF = 0;

  SDFCluster* c;

  // Reset visit flag and iteration count.
  // The iteration count is used in this scheduler to store the user's
  // requested number of iterations for each star.  It can be nonzero
  // only for atomic clusters.
  while ((c = nextClust++) != 0) {
    c->setVisit(0);
    if (!c->asBag()) {
      SDFAtomCluster* as = (SDFAtomCluster*) c;
      as->real().setIter(0);
      as->real().resetFirings();
    }
  }

  // Clear the list of stars for which pragmas have been registered.
  pragmaStars.initialize();

  nextClust.reset();
  while ((c = nextClust++) != 0) {
    if (c->asBag() || c->visited()) continue;

    SDFAtomCluster* as = (SDFAtomCluster*) c;
    // Skip clustering if either the star is not SDF or a target
    // pragma of type "firingsPerIteration" has been registered with it.
    // We have to call "pragmaRegistered" first to ensure that
    // pragmas will be registered for non-sdf stars.
    if (pragmaRegistered(as) || !as->isSDF()) {
      flagDDF = TRUE;
      continue;
    }

    // make a cluster.
    SDFClusterBag* curBag = galaxy.createBag();
    c->setVisit(1);
    if (!expandCluster(c, &galaxy, curBag)) return FALSE;
    curBag->absorb(c, &galaxy);
    nextClust.reset();
  }

  if (!flagDDF) amISDF = TRUE;

  if (logstrm) {
    *logstrm << " clustering SDF stars: completed...\n";
  }
  return TRUE;
}

///////////////////////////////////////////////////////////////////////
// Once we create an SDF cluster, we expand it as much as possible.
//
int DDFClustSched :: expandCluster (SDFCluster* c,
				    SDFClusterGal* galaxy,
				    SDFClusterBag* bag) {
  SDFClustPortIter nextp(*c);
  SDFClustPort* p;

  while((p = nextp++) != 0) {
    if (p->numTokens()) continue;	// if there is a delay, skip
    SDFCluster* s = p->far()->parentClust();
    if (s->visited()) continue;
    if (s->asBag()) continue;
    SDFAtomCluster* as = (SDFAtomCluster*) s;
    // Skip clustering if the farside star is not SDF or has a
    // "firingsPerIteration" pragma registered with it.
    if (!as->real().isSDF() || pragmaRegistered(as)) continue;
    s->setVisit(1);
    if (!expandCluster(s, galaxy, bag)) return FALSE;
    bag->absorb(s, galaxy);
  }
  return TRUE;
}

/*******************************************************************
		Main DDF scheduler routines
*******************************************************************/

// The strategy of the runtime DDF scheduler is as follows:
//
// One iteration, by default, consists of firing as many enabled
// and non-deferrable clustered actors as possible without firing any one
// actor more than once.  A deferrable actor is one whose
// output arcs all have enough data to satisfy the destination actors.
//
// E = enabled actors
// D = deferrable enabled actors
// S = source actors
// F = actors that have fired once already in this one iteration
//
// Note that S is a subset of E.
//
// One default iteration consists of:
//	F = 0
//	while (E-D-F != 0) {
//	  fire (E-D-F)
//	  F += E-D
//	  update E,D
//	}
//	if (F == 0) {
//	  // All enabled actors are deferrable.  Try the non-sources first.
//	  if (E-S != 0) {
//	    fire (E-S);
//	  } else {
//	    // E=S => unbounded memory. No policy will keep memory bounded,
//	    // because these source actors are all deferrable.
//	    fire (S);
//	  }
//	}
//	if (no stars fired above) deadlock;
// 
// This default iteration is defined to fire all clustered actors
// at most once.  A larger notion of an iteration can be specified using
// the targetPragmas mechanism to identify particular stars that must fire
// some specific number of times (greater than or equal to one) in
// each iteration.  This will be accomplished by executing enough default
// iterations until these specified numbers are met or exceeded.
//
// Note that if a targetPragma is specified for an SDF actor, then clustering
// is disabled for that actor.  This is because if we cluster the actor,
// then it becomes very difficult to control how many times it fires.
//
int DDFClustSched :: run() {
  if (realSched) {
    realSched->setStopTime(getStopTime());
    return realSched->run();
  }
  if (!galaxy()) {
    Error::abortRun("No galaxy to run");
    return FALSE;
  }
  
  if (haltRequested()) return FALSE;

  SDFClusterGalIter nextClust(*cgal);
  SDFCluster* c;

  while (numFiring < stopTime && !haltRequested()) {

    // Reset the firings count for all clusters.
    nextClust.reset();
    while ((c = nextClust++) != 0) {
      if (!c->asBag()) {
	SDFAtomCluster* as = (SDFAtomCluster*) c;
	as->real().resetFirings();
      }
    }

    // The following loop might be repeated if any of the stars in
    // it return zero in the enoughFirings() method.
    int doAgain;
    do {
      doAgain = FALSE;

      // reset visit flag
      nextClust.reset();
      while ((c = nextClust++) != 0) c->setVisit(0);

      // (1) Fire all enabled and non-deferrable actors at most once.
      //     This is done in multiple passes so that the choice of which
      //     actors to fire does not depend on the order in which they
      //     are examined.
      int firedOne = FALSE;
      int firedOneInPass;
      SequentialList enabledDefNonSources, defSources;
      do {
	firedOneInPass = FALSE;
	nextClust.reset();
	while ((c = nextClust++) != 0) {
	  if (c->visited()) continue;
	  int state = enabledState(c);
	  if (state == 1) { // enabled and non-deferrable
	    if (haltRequested() || !c->run()) return FALSE;
	    c->setVisit(1);
	    firedOne = TRUE;
	    firedOneInPass = TRUE;
	  } else if (state == 2) { // enabled, deferrable, non-source
	    // Construct a list of enabled, deferrable, non-source clusters
	    // for phase two of scheduling.
	    enabledDefNonSources.append(c);
	  } else if (state == 3) { // source, deferrable.
	    // Construct a list of deferrable sources for phase three.
	    defSources.append(c);
	  }
	}
      } while (firedOneInPass == TRUE);

      // (2) If the above yielded no firings at all, then we proceed to the
      //     step of firing all enabled, deferrable actors that are not sources.
      //     This is done in a single pass to catch only the problematic
      //     actors. To maintain determinacy, the list of enabled actors
      //     fired is the one constructed in phase 1 (i.e., the contents
      //     of the list does not change as we fire actors).
      if (!firedOne) {
	ListIter next(enabledDefNonSources);
	while ((c = (SDFCluster*)next++) != 0) {
	  if (haltRequested() || !c->run()) return FALSE;
	  c->setVisit(1);
	  firedOne = TRUE;
	}
      }

      // (3) If the above still yielded no firings, then the graph
      //     cannot be executed in bounded memory.  All enabled actors
      //     are deferrable sources.  We now simply fire
      //     all these sources.
      if (!firedOne) {
	ListIter next(defSources);
	while ((c = (SDFCluster*)next++) != 0) {
	  if (haltRequested() || !c->run()) return FALSE;
	  c->setVisit(1);
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
    } while (doAgain);

    // end of ONE iteration
    numFiring++;	

    // Check for buffer overflow.
    // FIXME: This should be optional, for efficiency.
    nextClust.reset();
    while ((c = nextClust++) != 0) {
      SDFClustPortIter nextPort(*c);
      SDFClustPort *p = 0;
      while ((p = nextPort++) != 0) {
	SDFClustPort* inp = p;
	while (inp->inPtr()) inp = inp->inPtr();
	if (p->isItInput()) {
	  int x = inp->real().numTokens();
	  if (x > maxToken) {
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
int DDFClustSched :: isSource(Star& s) {
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
int DDFClustSched :: enabledState(SDFCluster* s) {

  // If s is an atomic cluster (one star only), then the star might be
  // a DDF star. Check to see whether it has a waitPort (a port it is
  // waiting for data on), and whether that port has enough data.
  // FIXME: Note that this code depends on the fact that if
  // s is not an SDFClusterBag, then it must be an SDFAtomCluster.
  // This is true now, but if more classes are added, things could change.
  // If s is a bag, then s is an SDF cluster.

  if (!s->asBag()) {
    SDFAtomCluster* as = (SDFAtomCluster*) s;
    DataFlowStar& ds = as->real();
    DFPortHole* wp = ds.waitPort();
    if (wp) {
      // If fewer tokens are available than are needed, not enabled.
      if (wp->numTokens() < ds.waitTokens()) return 0;

      // If there are enough inputs, and the output ports do not have
      // enough data already, enabled and not deferrable.
      if (!isOutputDeferrable(s)) return 1;

      // The actor is enabled and deferrable.  It cannot possibly be
      // a source, since it has a wait port.
      return 2;
    }
  }

  // If s is not a cluster and has no waitPort, then check all inputs
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
// Check to see whether a cluster is blocked on input.
// Return 1 if it is, 0 otherwise.  So a return value of zero
// means that the cluster is enabled (which could means it's a source).
//
int DDFClustSched :: blockedOnInput(SDFCluster* s) {
  SDFClustPortIter nextPort(*s);
  SDFClustPort *p;
  while ((p = nextPort++) != 0) {
    SDFClustPort* inp = p;
    if (p->isItInput()) {
      // If the arc is over the limit, flag it.
      if (p->numTokens() > maxToken) overFlowArc = p;

      while (inp->inPtr()) inp = inp->inPtr();

      if (inp->real().numTokens() < p->numIO()) {
	return 1;
      }
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////////////
// Check to see whether all the outputs have enough data already to
// satisfy downstream arcs.  If there are no outputs, always return FALSE
// (this prevents deferring of sink actors).
//
int DDFClustSched :: isOutputDeferrable(SDFCluster* s) {
  SDFClustPortIter nextPort(*s);
  SDFClustPort *p;
  int outputFound = FALSE;
  while ((p = nextPort++) != 0) {
    if (p->isItInput()) continue;
    SDFClustPort* outp = p;
    outputFound = TRUE;

    // Find the actual porthole.
    while (outp->inPtr()) outp = outp->inPtr();
    DFPortHole& dp = outp->real();

    // If output is not connected, continue
    PortHole* far = dp.far();
    if (!far) continue;

    DataFlowStar* ds = (DataFlowStar*) far->parent();
    DFPortHole* wp = ds->waitPort();
    if (wp && (far == (PortHole*) wp)) {
      // The downstream actor is waiting for data on this port.
      // If the number of tokens available is too small...
      if (wp->numTokens() < ds->waitTokens()) return 0;
    } else if (wp) {
      // The downstream actor is waiting for data
      // on another port.
      continue;
    } else {
      // The downstream actor is waiting for data on this
      // and other ports (probably an SDF actor).
      if (p->far()->numIO() > dp.numTokens()) return 0;
    }
  }
  if (outputFound) return 1;
  else return 0;
}
		
