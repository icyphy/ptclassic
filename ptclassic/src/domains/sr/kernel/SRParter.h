/* -*- C++ -*- */

#ifndef _SRParter_h
#define _SRParter_h

/* Version $Id$

@Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.

Permission is hereby granted, without written agreement and without
license or royalty fees, to use, copy, modify, and distribute this
software and its documentation for any purpose, provided that the
above copyright notice and the following two paragraphs appear in all
copies of this software.

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

						PT_COPYRIGHT_VERSION_2
						COPYRIGHTENDKEY

    Author:     S. A. Edwards
    Created:    28 October 1996

 */

#ifdef __GNUG__
#pragma interface
#endif

class SRDependencyGraph;
class SRRecursiveSchedule;
class Set;
class SetIter;

// The different partitioning routines
typedef enum SRPartTypeEnum {
  SRPartOneT, SRPartInOutT, SRPartExactT, SRPartSweepT
} SRPartType;

/**********************************************************************

  Abstract partition generator

**********************************************************************/
class SRPart {
protected:

  // The set being partitioned
  Set * partSet;

  // The associated dependency graph
  SRDependencyGraph * dgraph;

  // Flag indicating when partitions giving only one SCC should be ignored
  static int ignoreSimpleFlag;

public:

  SRPart( Set & s, SRDependencyGraph & g ) { partSet = &s; dgraph = &g; }
  virtual ~SRPart();
 
  // Restart the partitioner from the beginning
  virtual void init() = 0;

  // Return the next partition no greater than the given size
  // @Description Is is the responsibility of the caller to free the
  // returned set.
  virtual Set * next( int ) = 0;

  // Return whether partitions giving a single SCC should be ignored
  static int ignoreSimple() { return ignoreSimpleFlag; }
};

/**********************************************************************

  Partitioner that generates all single vertex partitions

  @Description Essentially, another implementation of the SetIter class

**********************************************************************/
class SRPartOne : public SRPart {
public:

  SRPartOne( Set & s, SRDependencyGraph & g ) : SRPart(s,g) {
    ignoreSimpleFlag = 0;
  };

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
  SRPartInOut( Set & s, SRDependencyGraph & g ) : SRPart(s,g) {
    minset = 0;
    ignoreSimpleFlag = 0;
  };
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
  SRPartExact( Set & s, SRDependencyGraph & g ) : SRPart(s,g) {
    vertex = vindex = 0;
    ignoreSimpleFlag = 1;
  };
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

 Greedily generates all borders starting from each vertex

 @Description Starting from each vertex in the graph, absorb the cheapest
 vertex and return the border of the set.

 **********************************************************************/
class SRPartSweep : public SRPart {
public:
  SRPartSweep( Set & s, SRDependencyGraph & g ) : SRPart(s,g) {
    kernel = 0;
    sgIter = 0;
    ignoreSimpleFlag = 1;
  };

  ~SRPartSweep();
  void init();
  Set * next( int );

private:
  // Iterates over the vertices in the subgraph
  SetIter * sgIter;

  // The kernel, grown from each vertex
  Set * kernel;

  Set * border();

  // Flag indicating when all vertices have been exhausted
  int done;
};


/**********************************************************************

  A wrapper/selector for the partitions class

**********************************************************************/
class SRParter {

private:

  // The partitioning iteration routine
  SRPart * mypart;

  // Which part routine to use
  static SRPartType parter;

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

  static void setParter( const char * );

};

#endif
