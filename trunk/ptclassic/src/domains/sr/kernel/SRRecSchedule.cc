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
#include "SRStar.h"
#include <stream.h>
#include <assert.h>



// Create a new schedule by allocating the arrays
SRRecursiveSchedule::SRRecursiveSchedule( SRDependencyGraph & g )
{
  mygraph = &g;

  vertex = new int[ g.vertices() * 5 ];
  partSize = vertex + g.vertices();
  headSize = partSize + g.vertices();
  parSize = headSize + g.vertices();
  repCount = parSize + g.vertices();
}

// Destroy a schedule by deleting the arrays
SRRecursiveSchedule::~SRRecursiveSchedule()
{
  delete [] vertex;
}

// Return the cost of the schedule
int SRRecursiveSchedule::cost() const
{
  int total = 0;
  int v = 0 ;
  while ( v < mygraph->vertices() ) {
    total += partitionCost( v );
  }
  return total;
}

// Return the cost of one partition
//
// @Description The index v is updated to point to the next partition
int SRRecursiveSchedule::partitionCost( int & v ) const
{

  if ( partSize[v] == parSize[v] ) {

    // An "execute once" parallel block

    int cost = partSize[v];
    v += partSize[v];
    return cost;
  }

  // A partition with a tail and perhaps a head

  int tailEnd = v + partSize[v];
  int headEnd = v + headSize[v];
  int rc = repCount[v];

  int headCost = 0;
  while ( v < headEnd ) {
    headCost += parSize[v];
    v += parSize[v];
  }
  int tailCost = 0;
  while ( v < tailEnd ) {
    tailCost += partitionCost( v );
  }

  return headCost * rc + tailCost * ( rc + 1 );
  
}

// Return a compact printed form of the schedule
StringList SRRecursiveSchedule::print() const
{
  StringList out;

  // Print each partition
 
  int v = 0;
  while ( v < mygraph->vertices() ) {
    printOnePartition(v,out);
  }

  return out;
}

// Print the schedule for one block and update the index
void
SRRecursiveSchedule::printOneBlock( int & v /* first index of the block */,
				    StringList & out ) const
{
  if ( parSize[v] == 1 ) {
    out << vertex[v] << " ";
    v++;
  } else {
    out << "[ ";
    for (int j = parSize[v] ; --j >= 0 ; ) {
      out << vertex[v] << " ";
      v++;
    }
    out << "]";
  } 
}

// Print the schedule for one partition and update the index
void
SRRecursiveSchedule::printOnePartition(int & v /* partition's first vertex */,
				      StringList & out) const
{
  if ( partSize[v] == parSize[v] ) {
    printOneBlock( v, out );
  } else {

    int et = v + partSize[v];

    if ( headSize[v] > 0 ) {

      // Non-separable partition with a head and a repetition count

      int rc = repCount[v];

      out << "( ";
      int eh = v + headSize[v];
      while ( v < eh ) {
	printOneBlock( v, out );
      }
      out << ". ";
      while ( v < et ) {
	printOnePartition( v, out );
      }
      out << ")^" << rc << " ";

    } else {

      // Separable partition

      while ( v < et ) {
	printOnePartition( v, out );
      }

    }
  }
}

// Add a single vertex to evaluate at the given index and return the next index
int SRRecursiveSchedule::addSingleVertex( int i, int v )
{
  vertex[i] = v;
  partSize[i] = 1;
  parSize[i] = 1;
  headSize[i] = 0;
  repCount[i] = 0;
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
    j++;
  }

  partSize[i] = r;
  parSize[i] = headSize[i] = repCount[i] = s.cardinality();

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
    parSize[index + i] = s.parSize[index + i];
    headSize[index + i] = s.headSize[index + i];
    repCount[index + i] = s.repCount[index + i];
  }
}

/**********************************************************************
Determine the "pushable range" for a particular vertex

@Description  Return 1 if vertex was found, 0 otherwise.

<P> The "pushable range" for a vertex in a head is its tail.
The range for a vertex in a tail is every vertex up to the beginning
of its tail.  Start is the first, end is one more than the last.  start = end is an empty range.

<P> For example,
<PRE>
Index:      0     1   2 3 4     5 6
Schedule: ( 6 . ( 4 . 1 0 2 )^1 3 5 )^1
0                     2*3
1                     2 
2                     2***4
3                 1*************5
4                     2*********5
5                 1***************6
6                 1*****************7
</PRE>

**********************************************************************/

int
SRRecursiveSchedule::findPushRange( int v /* the vertex to find */,
				    int & start /* the starting range */,
				    int & end /* the ending range */,
				    int & index /* the index where it was found */)
{
  for ( int i = 0 ; i < mygraph->vertices(); i += partSize[i] ) {
    if ( parSize[i] == partSize[i] ) {

      // A simple parallel block -- check its contents

      for ( int k = 0 ; k < parSize[i] ; k++ ) {
	if ( vertex[i + k] == v ) {

	  // The vertex is in the tail of this partition, so the push
	  // range is the beginning of the tail up to the vertex

	  start = 0;
	  end = i + k;
	  index = i + k;
	  return start != end;
	}
      }
      
    } else {
      if ( findRange(v, start, end, index, i ) ) {
	assert( start <= end );
	return 1;
      }
    }
  }

  return 0;

}

int
SRRecursiveSchedule::findRange( int v /* the vertex to find */,
				int & start /* the starting range */,
				int & end /* the ending range */,
				int & index /* where it was found */,
				int i /* where to start searching */ )
{
  int j;

  // Look for the vertex in the head of this partition

  for ( j = 0 ; j < headSize[i] ; j++ ) {
    if ( vertex[i + j] == v ) {

      // The vertex is in the head of this partition, so the push range is
      // its tail

      start = i + headSize[i];
      end = i + partSize[i];
      index = i + j;
      return start != end;

    }
  }

  // Look for the vertex in the tail of this partition
 
  for ( j = headSize[i] ; j < partSize[i] ; j += partSize[i+j] ) {
    if ( parSize[i + j] == partSize[i + j] ) {

      // A simple parallel block -- check its contents

      for ( int k = 0 ; k < parSize[j] ; k++ ) {
	if ( vertex[i + j + k] == v ) {

	  // The vertex is in the tail of this partition, so the push
	  // range is the beginning of the tail up to the vertex

	  start = i + headSize[i];
	  end = i + j + k;
	  index = i + j + k;
	  return start != end;
	}
      }

    } else {
      if ( findRange(v, start, end, index, i + j ) ) {
	return 1;
      }
    }
  }

  return 0;

}

// Condense a vertex into the best parallel block
//
// @Description Find the vertex and its push range using findPushRange,
// try to push it back as far as possible, then push it towards the best
// parallel block.
int
SRRecursiveSchedule::mergeVertex(int v /* the vertex to be merged */)
{
  cout << "Merging vertex " << v << '\n';

  // Find the push range for this vertex

  int start;
  int end;
  int index;

  if ( !findPushRange(v, start, end, index) ) {
    cout << "Empty push range\n";
    return 0;
  }

  cout << "The range is " << start << ".." << end << '\n';
  cout << "The vertex was located at " << index << '\n';

  // Try to push the vertex to the left as far as possible

  int bestIndex = end;

  while ( bestIndex > start ) {
    // Get the output we're trying to push to the left of
    int outputVertex = vertex[bestIndex - 1];

    // Check to see if it appears in the input list of the moving
    // vertex

    for (int e = mygraph->backwardEdges(vertex[index]) ; --e >= 0 ; ) {
      if ( outputVertex == mygraph->source(vertex[index],e) ) {

	// output was one of the inputs

	break;
      }
    }

    if ( e >= 0 ) {
      break;
    }

    // The output was not one of the moving vertex's inputs

    bestIndex--;

  }

  cout << "Pushed it as far left as " << bestIndex << '\n';

  SRStar * outputStar = mygraph->star(vertex[index]);

  // Push the vertex right until finding an output on the same star

  while ( bestIndex < end ) {
    if ( mygraph->star(vertex[bestIndex]) == outputStar ) {
      break;
    }
    bestIndex++;
  }

  assert ( bestIndex <= end );

  if ( bestIndex == end ) {
    cout << "Couldn't find anything on the same star to the right\n";
    return 0;
  }

  cout << "Found a matching star at " << bestIndex << '\n';

  cout << "Matched " << mygraph->star(vertex[index])->name() << " and "
       << mygraph->star(vertex[bestIndex])->name() << '\n';

  return 1;

}
