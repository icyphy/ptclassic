/* -*- C++ -*- */

#ifndef _SRRecusiveSchedule_h
#define _SRRecusiveSchedule_h

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
    Created:    23 October 1996

 */

#ifdef __GNUG__
#pragma interface
#endif

class SRDependencyGraph;
class StringList;
class Set;

/**********************************************************************

  A scheduler for the SR domain's recursive schedule

  @Description This is a parenthesized permutation of the vertices in
  the dependency graph where the beginning of each parenthesized group
  may have zero or more vertices that are to be evaluated directly.

  <P> The syntax is as follows:
<PRE>
  s ->  i                 Single output
     |  [i1 i2 ... ik ]   Outputs to be evaluated in parallel
     |  s1 s2             Schedules to be evaluated in series
     |  ( s1 . s2 )^n     To be evaluated s2 s1 s2 ... s1 s2 (n times)
</PRE>

  <P> Four arrays represent this structure.  For example:
<PRE>
 ([1   2]. 3 ( 4.  5 [ 6   7 ] 8) (9.(10. 11) 12 ))     Schedule
   1   2   3   4   5   6   7   8   9  10  11  12        vertex
  12   0   1   5   1   2   0   1   4   2   1   1        partSize
   2   0   0   1   0   0   0   0   1   1   0   0        headSize
   2   0   1   1   1   2   0   1   1   1   1   1        parSize
   2   0   0   1   0   0   0   0   1   1   0   0        repCount
</PRE>

   <UL>
   <LI> vertex: the vertex to evaluate
   <LI> partSize: the size of the whole partition
   <LI> headSize: the size of the head of the partition
   <LI> parSize: the size of the parallel block
   <LI> repCount: the number of repetitions for the head
   </UL>

   partSize = parSize implies this is a simple parallel block. <BR>
   headsize > 0 implies this is a non-separable partition with a head <BR>

**********************************************************************/
class SRRecursiveSchedule {
public:

  SRRecursiveSchedule( SRDependencyGraph & );
  ~SRRecursiveSchedule();

  StringList print() const;

  //  StringList printSchedule() const;

  StringList printVerbose() const;

  int addSingleVertex( int, int );

  int addPartition( int, int, Set & );

  void getSection( int, int, SRRecursiveSchedule & );

  int cost() const;

  int findPushRange( int, int &, int &, int & );

  int mergeVertex( int );

  void splitParallelBlocks();

  int deleteIndex( int );

  int insertVertexAtIndex( int, int );

  void runSchedule() const;

private:
  // The dependency graph for which this schedule is computed
  SRDependencyGraph * mygraph;

  // The array of vertex indices for the sequence
  int * vertex;

  // The array of partition sizes for each vertex
  int * partSize;

  // The size of the head of the partition
  int * headSize;

  // The size of the parallel block
  int * parSize;

  // The number of repetitions for this block
  int * repCount;

  void printOnePartition(int &, StringList & ) const;

  void printOneBlock( int &, StringList & ) const;

  void printVerbosePartition( int &, int, StringList & ) const;

  void printVerboseBlock( int &, int, StringList & ) const;

  void runOnePartition( int & ) const;

  void runOneBlock( int & ) const;

  int partitionCost( int & ) const;

  int findRange( int, int &, int &, int &, int );
};

#endif
