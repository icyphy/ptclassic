/* -*- C++ -*- */

#ifndef _SRParter_h
#define _SRParter_h

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
    Created:    28 October 1996

 */

#ifdef __GNUG__
#pragma interface
#endif

class SRDependencyGraph;
class SRRecursiveSchedule;
class Set;

// The different partitioning routines
typedef enum SRPartTypeEnum {
  SRPartOneT, SRPartInOutT, SRPartExactT
} SRPartType;

/**********************************************************************

  Abstract partition generator

**********************************************************************/
class SRPart {
public:

  SRPart( Set & s, SRDependencyGraph & g ) { partSet = &s; dgraph = &g; }
  virtual ~SRPart();
 
  // Restart the partitioner from the beginning
  virtual void init() = 0;

  // Return the next partition no greater than the given size
  // @Description Is is the responsibility of the caller to free the
  // returned set.
  virtual Set * next( int ) = 0;

protected:

  // The set being partitioned
  Set * partSet;

  // The associated dependency graph
  SRDependencyGraph * dgraph;
  
};

/**********************************************************************

  Partitioner that generates all single vertex partitions

  @Description Essentially, another implementation of the SetIter class

**********************************************************************/
class SRPartOne : public SRPart {
public:

  SRPartOne( Set & s, SRDependencyGraph & g ) : SRPart(s,g) {};
  void init();
  Set * next( int );

private:
  // Index of the most recent member of the set
  int vertexIndex;
  
};

/**********************************************************************

  Generates an inset or outset of minimum size

  @Description This returns a single partition that is the incoming or
  outgoing vertex set of minimum cardinality
  
**********************************************************************/
class SRPartInOut : public SRPart {
public:
  SRPartInOut( Set & s, SRDependencyGraph & g ) : SRPart(s,g) {};
  ~SRPartInOut();
  void init();
  Set * next( int );

private:
  // Set of minimum cardinality
  Set * minset;
  
};

/**********************************************************************

  Generates all possible partitions

  @Description generates all partitions of size one, then all of size
  two, etc.

**********************************************************************/
class SRPartExact : public SRPart {
public:
  SRPartExact( Set & s, SRDependencyGraph & g ) : SRPart(s,g) {};
  ~SRPartExact();
  void init();
  Set * next( int );

private:
  // Size of the partitions currently being generated
  int partsize;

  // Maximum size of the partition set
  int maxpart;

  // Array of vertices in the subgraph to be partitioned
  int * vertex;

  // Array of indices into the vertex array
  int * vindex;

};


/**********************************************************************

  A wrapper/selector for the partitions class

**********************************************************************/
class SRParter {

private:

  // The partitioning iteration routine
  SRPart * mypart;

public:

  SRParter( Set &, SRDependencyGraph & );

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
