/* -*- C++ -*- */

#ifndef _SRRecusiveSchedule_h
#define _SRRecusiveSchedule_h

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

class SRDependencyGraph;
class StringList;
class Set;

/**********************************************************************

  A scheduler for the SR domain's recursive schedule

  @Description This is a parenthesized permutation of the vertices in
  the dependency graph where the beginning of each parenthesized group
  may have zero or more vertices that are to be evaluated directly.

  <P> For example, the schedule
  <PRE>
     _ _    _         _
    (1 2 3 (4 5 6) 7 (8 9))
  </PRE>
  corresponds to the sequence

  <PRE>
  (3 (5 6) 4 (5 6) 7 9 8 9) 1 2
  (3 (5 6) 4 (5 6) 7 9 8 9) 1 2
  (3 (5 6) 4 (5 6) 7 9 8 9)
  </PRE>

**********************************************************************/
class SRRecursiveSchedule {
public:

  SRRecursiveSchedule( SRDependencyGraph & );
  ~SRRecursiveSchedule();

  StringList print() const;

  //  StringList printSchedule() const;

  int addSingleVertex( int, int );

  int addPartition( int, int, Set & );

  void getSection( int, int, SRRecursiveSchedule & );

private:
  // The dependency graph for which this schedule is computed
  SRDependencyGraph * mygraph;

  // The array of vertex indices for the sequence
  int * vertex;

  // The array of partition sizes for each vertex
  int * partSize;

  // The array of things to be directly evaluated for each vertex
  int * directSize;

  int printOnePartition(int, StringList & ) const;

  //  void scheduleOnePartition(int, StringList & ) const;
  
};

#endif
