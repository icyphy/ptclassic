static const char file_id[] = "SRRecursiveScheduler.cc";

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
#pragma implementation
#endif

#include "SRRecursiveSchedule.h"
#include "SRDependencyGraph.h"
#include "StringList.h"
#include "Set.h"

// Create a new schedule by allocating the arrays
SRRecursiveSchedule::SRRecursiveSchedule( SRDependencyGraph & g )
{
  mygraph = &g;

  vertex = new int[ g.vertices() ];
  partSize = new int[ g.vertices() ];
  directSize = new int[ g.vertices() ];
}

// Destroy a schedule by deleting the arrays
SRRecursiveSchedule::~SRRecursiveSchedule()
{
  delete [] vertex;
  delete [] partSize;
  delete [] directSize;
}

// Return a compact printed form of the schedule
StringList SRRecursiveSchedule::print() const
{
  StringList out;

  // Print each partition
 
  int v = 0;
  while ( v < mygraph->vertices() ) {
    v = printOnePartition( v, out );
  }

  return out;
}

// Print the schedule for one partition, return the index of the next
int
SRRecursiveSchedule::printOnePartition(int v /* partition's first vertex */,
				      StringList & out) const
{
  if ( partSize[v] == 1 ) {
    out << vertex[v] << " ";
  } else {
    int i = v;
    out << "( ";
    if ( directSize[v] > 0 ) {
      while ( i < v + directSize[v] ) {
	out << vertex[i] << " ";
	i++;
      }
      out << ". ";
    }
    while ( i < v + partSize[v] ) {
      i = printOnePartition(i, out);
    }
    out << ") ";
  }

  return v + partSize[v];
}

// Add a single vertex to evaluate at the given index and return the next index
int SRRecursiveSchedule::addSingleVertex( int i, int v )
{
  vertex[i] = v;
  partSize[i] = 1;
  directSize[i] = 0;
  return i+1;
}

// Add a partition to evaluate at the given index and return the next index
int
SRRecursiveSchedule::addPartition( int i,
				   int r /* Total partition size */,
				   Set & s /* Partition set */)
{
  // Record the vertices in the partition set
  int v;
  int j = i;
  SetIter next(s);
  while ( (v = next++) >= 0 ) {
    vertex[j] = v;
    directSize[j] = 0;
    partSize[i] = 1;
    j++;
  }

  directSize[i] = s.cardinality();
  partSize[i] = r;

  return j;
}

// Copy a partition from one schedule to another
void
SRRecursiveSchedule::getSection( int index /* Starting index of the section */,
				 int size /* Number of vertices to get */,
				 SRRecursiveSchedule & s )
{
  for ( int i = size ; --i >= 0 ; ) {
    vertex[index + i] = s.vertex[index + i];
    partSize[index + i] = s.partSize[index + i];
    directSize[index + i] = s.directSize[index + i];
  }
}

