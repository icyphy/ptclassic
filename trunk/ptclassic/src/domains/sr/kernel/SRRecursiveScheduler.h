/* -*- C++ -*- */

#ifndef _SRRecursiveScheduler_h
#define _SRRecursiveScheduler_h

/* Version $Id$

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

    Author:     S. A. Edwards
    Created:    23 October 1996

 */

#ifdef __GNUG__
#pragma interface
#endif

#include "Scheduler.h"

class SRDependencyGraph;
class SRRecursiveSchedule;
class Set;

/**********************************************************************

 The recursive scheduler

 @Description

 **********************************************************************/
class SRRecursiveScheduler : public Scheduler
{
public:
  SRRecursiveScheduler();
  ~SRRecursiveScheduler();

  const char* domain() const;

  void setup();

  void setStopTime(double);

  void resetStopTime(double);

  // Return the stopping time of the simulation
  double getStopTime() { return double(numInstants); }

  int run();

  virtual void runOneInstant();

  // The "time" of each instant, used when interfacing with a timed domain
  double schedulePeriod;

  StringList displaySchedule();  

protected:
  // Number of instants to execute
  int numInstants;

  // Number of instants already executed
  int numInstantsSoFar;

  virtual int computeSchedule( Galaxy & );

  int mincost( Set &, int, SRRecursiveSchedule &, int );

private:

  // The dependency graph of the galaxy
  SRDependencyGraph * dgraph;

  SequentialList * SCCsOf(Set &);

  void fDFSVisit( int, Set &, int &, int * );
  void bDFSVisit( int, Set &, Set &);

};

// The different partitioning routines
typedef enum SRPartTypeEnum { SRPartOneT } SRPartType;

/**********************************************************************

  The generic SRpart class

**********************************************************************/
class SRPart {
public:

  SRPart( Set & s ) { partSet = &s; }
 
  // Restart the partitioner from the beginning
  virtual void init() = 0;

  // Return the next partition no greater than the given size
  // @Description Is is the responsibility of the caller to free the
  // returned set.
  virtual Set * next( int ) = 0;

protected:

  // The set being partitioned
  Set * partSet;
  
};

/**********************************************************************

  Partitioner that generates all single vertex partitions

  @Description Essentially, another implementation of the SetIter class

**********************************************************************/
class SRPartOne : public SRPart {
public:
  SRPartOne( Set & s ) : SRPart(s) {};
  void init();
  Set * next( int );

private:
  // Index of the most recent member of the set
  int vertexIndex;
  
};

/**********************************************************************

  A wrapper/selector for the partitions class

**********************************************************************/
class SRParter {

private:

  // The partitioning iteration routine
  SRPart * mypart;

public:

  SRParter( Set & );

  // Destroy the associated partitioner
  ~SRParter() { delete mypart; }

  // Restart the iterator
  void init() { mypart->init(); }

  // Return the next partition no greater than the given size
  // @Description Is is the responsibility of the caller to free the
  // returned set.
  Set * next( int b /* Maximum allowed size of the partition */ ) {
    return mypart->next(b);
  }

  // Which part routine to use
  static SRPartType parter;

};

#endif
