/* -*- C++ -*- */

#ifndef _SRStaticScheduler_h
#define _SRStaticScheduler_h

/*  Version $Id$

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

    Author:	S. A. Edwards
    Created:	7 May 1996

*/

#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"

class SRStar;

// An SRScheduleCluster--a sequential list of stars to fire

class SRScheduleCluster : private SequentialList {
  friend class SRSchedClusterIter;

public:
  SRScheduleCluster() { numIterations = 1; }

  void append( SRStar * s ) { SequentialList::append( s ); }
  void prepend( SRStar * c ) { SequentialList::prepend( c ); }

  // Return the size
  SequentialList::size;

  // The number of times this cluster is to be executed
  int numIterations;
};


// An SRSchedule--a sequential list of clusters to execute

class SRSchedule : private SequentialList {
  friend class SRSchedIter;

public:

  // Add a new cluster to the schedule
  void append( SRScheduleCluster * c ) { SequentialList::append( c ); }

  void prepend( SRScheduleCluster * c ) { SequentialList::prepend( c ); }

  SRScheduleCluster * removeTail() {
    return (SRScheduleCluster *) SequentialList::getTailAndRemove();
  }

  // Return the number of clusters
  SequentialList::size;

  // Return a printable version of the schedule
  StringList printVerbose() const;

};

class SRStaticScheduler : public Scheduler
{
  friend class SRSchedIter;
public:
  SRStaticScheduler();

  // Domain identification.
  /*virtual*/ const char* domain() const;

  // Initialization.
  /*virtual*/ void setup();


  // Set the stopping time
  /*virtual*/ void setStopTime(double);

  // Set the stopping time when inside a Wormhole.
  /*virtual*/ void resetStopTime(double);

  // Return the stopping time.
  /*virtual*/ double getStopTime() { return double(numInstants); }


  // Run (or continue) the simulation
  /*virtual*/ int run();

  // Run the simulation for an instant
  virtual void runOneInstant();

  // The "time" of each instant, used when interfacing with a timed domain
  double schedulePeriod;

  // Return a printed representation of the schedule
  StringList displaySchedule();

  // Indices for the flags used during scheduling

  enum flags {
    sccDepth = 0,	// Used within the SCC decomposition routine
    cutIndex = 0	// Used within the feedback arc cut routine
  };

protected:
  // Number of instants to execute
  int numInstants;

  // Number of instants already executed
  int numInstantsSoFar;

  // Schedule the galaxy
  virtual int computeSchedule( Galaxy & );

  // The schedule for this galaxy
  SRSchedule mySchedule;

private:

  // Decompose the graph into strongly-connected components
  // (stored as the schedule)
  void SCCDecompose( Galaxy & );

  // Routine for building strongly-connected components
  int SCCVisit( SRStar *, Stack &, int & index );

  // Reorder the stars in each cluster to minimize the number of feedback arcs
  void cutSCCs( Galaxy & );

};

// Iterator for SRScheduleCluster

class SRSchedClusterIter : private ListIter {
public:
  SRSchedClusterIter(const SRScheduleCluster & s) : ListIter(s) {}
  SRStar * next() { return (SRStar *)ListIter::next();}
  SRStar * operator++(POSTFIX_OP) { return next();}
  ListIter::reset;
};

// Iterator for SRSchedule

class SRSchedIter : private ListIter {
public:
  SRSchedIter(const SRSchedule & s) : ListIter(s) {}
  SRSchedIter(SRStaticScheduler & sch) : ListIter(sch.mySchedule) {}
  SRScheduleCluster * next() { return (SRScheduleCluster *)ListIter::next();}
  SRScheduleCluster * operator++(POSTFIX_OP) { return next();}
  ListIter::reset;
};

#endif


