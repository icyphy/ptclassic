static const char file_id[] = "SRRecScheduler.cc";

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
#pragma implementation
#endif

#include "SRRecSchedule.h"
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
//
// @Description The cost is defined as the total 
// number of <EM>outputs</EM> evaluated in a single run of the schedule.

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
int
SRRecursiveSchedule::partitionCost( int & v ) const
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
StringList
SRRecursiveSchedule::print() const
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
    out << "] ";
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

// Return the Tcl list form of this schedule
//
// @Description This follows the formatting conventions described in
// <A HREF="http://ptolemy.eecs.berkeley.edu/~ptdesign/newPtolemyInfrastructure/schedules.html">
// ``Specifying Schedules''</A>.
StringList
SRRecursiveSchedule::printVerbose() const
{
  StringList out;

  // Print each partition

  int v = 0;
  while ( v < mygraph->vertices() ) {
    printVerbosePartition(v, 2, out );
  }

  return out;
}

// Print one partition in the schedule
void
SRRecursiveSchedule::printVerbosePartition(
	int & v /* partition's first vertex */,
	int indent /* indentation level */,
	StringList & out) const
{
  int i;

  if ( partSize[v] == parSize[v] ) {

    printVerboseBlock( v, indent, out );

  } else {
    int et = v + partSize[v];
    if ( headSize[v] > 0 ) {

      // Print a non-separable partition with a head and repetition count
     
      for ( i = indent ; --i >= 0 ; ) out << " ";
      out << "{ repeat " << repCount[v] << "\n";

      // Print the head 

      for ( i = indent + 2; --i >= 0 ; ) out << " ";
      out << "{ head\n";
      
      int eh = v + headSize[v];

      while ( v < eh ) {
	printVerboseBlock( v, indent + 4, out );
      }

      for ( i = indent + 2; --i >= 0 ; ) out << " ";
      out << "}\n";

      // Print the tail

      while ( v < et ) {
	printVerbosePartition( v, indent + 2, out );
      }

      for ( i = indent; --i >= 0 ; ) out << " ";
      out << "}\n";

    } else {

      // Print a separable partition

      while ( v < et ) {
	printVerbosePartition( v, indent, out );
      }

    }      
  }
}

// Print one block in the schedule
void
SRRecursiveSchedule::printVerboseBlock(
	int & v /* partition's first vertex */,
	int indent /* indentation level */,
	StringList & out) const
{
  int i;

  SRStar * s;

  for ( i = indent; --i >= 0 ; ) out << " ";
  s = mygraph->star(vertex[v]);
  out << "{ fire " << s->name();

  for ( int j = parSize[v] ; --j >= 0 ; ) {

    out << " { output ";

    // All the vertices in this parallel block should be on the same star
    assert( s == mygraph->star(vertex[v]) );
   
    if ( mygraph->port(vertex[v]) ) {
      out << (mygraph->port(vertex[v]))->name();
    } else {
      out << "(fictitious)";
    }

    out << " }";   

    v++;
  
  }

  out << " }\n";
  
}

// Run the schedule by calling the run method of each star
//
// @Description Go through the schedule in order.  For each parallel block,
// call the run method of its star.  For this to be correct, each parallel
// block must contain outputs on exactly one star.  Calling splitParallelBlocks
// can ensure this.
//
// @SeeAlso splitParallelBlocks runOnePartition runOneBlock
void
SRRecursiveSchedule::runSchedule() const
{

  int v = 0;
  while ( v < mygraph->vertices() ) {
    runOnePartition(v);
  }

}

// Run a single partition
void
SRRecursiveSchedule::runOnePartition(int & v /* partition's first vertex */ )
const
{
  if ( partSize[v] == parSize[v] ) {

    // Parallel block -- run it

    runOneBlock( v );

  } else {

    int et = v + partSize[v];

    if ( headSize[v] > 0 ) {

      // Non-separable partition with a head and a repetition count

      int head = v;
      int tail = v + headSize[v];
      int rc = repCount[v];

      // Run the tail once

      for ( v = tail ; v < et ; ) {
	runOnePartition( v );
      }

      // Run the head then the tail as many times as the repetition count

      for ( ; --rc >= 0 ; ) {

	// Run the head 

	for ( v = head ; v < tail ; ) {
	  runOneBlock( v );
	}

	for ( ; v < et ; ) {
	  runOnePartition( v );
	}
	
      }

    } else {

      // Separable partition -- run it

      for ( ; v < et ; ) {
	runOnePartition( v );
      }

    }
  }
}

// Run a parallel block
//
// @Description Call the run method of the star at the first index of the block
// and return
void
SRRecursiveSchedule::runOneBlock(int & v /* first index of the block */) const
{
  SRStar * s = mygraph->star(vertex[v]);
  // cout << "Firing " << s->name() << "\n";
  s->run();
  v += parSize[v];
}

// Add a single vertex to evaluate at the given index and return the next index
int
SRRecursiveSchedule::addSingleVertex( int i, int v )
{
  vertex[i] = v;
  partSize[i] = 1;
  parSize[i] = 1;
  headSize[i] = 0;
  repCount[i] = 0;
  return i+1;
}

// Add a partition to evaluate at the given index and return the next index
//
// @Description Add a partition of the given size, with a single, fully
// parallel head containing every member of the partition set s.  The vertices
// in the new parallel head come in the order given by SetIter.
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
    partSize[j] = parSize[j] = headSize[j] = repCount[j] = 0;
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

@SeeAlso findRange

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

// Called by findPushRange
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

      for ( int k = 0 ; k < parSize[i + j] ; k++ ) {
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
// parallel block.  Vertices are moved with insertVertexAtIndex.
//
// @SeeAlso findPushRange insertVertexAtIndex
int
SRRecursiveSchedule::mergeVertex(int v /* the vertex to be merged */)
{
  // cout << "Merging vertex " << v << '\n';

  // Find the push range for this vertex

  int start;
  int end;
  int index;

  if ( !findPushRange(v, start, end, index) ) {
    // cout << "Empty push range\n";
    return 0;
  }

  // cout << "The range is " << start << ".." << end << '\n';
  // cout << "The vertex was located at " << index << '\n';

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

  // cout << "Pushed it as far left as " << bestIndex << '\n';

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
    // cout << "Couldn't find anything on the same star to the right\n";
    return 0;
  }

  // cout << "Found a matching star at " << bestIndex << '\n';

  // cout << "Matched " << mygraph->star(vertex[index])->name() << " and "
  //     << mygraph->star(vertex[bestIndex])->name() << '\n';

  // cout << "Deleted Schedule: ";

  if ( deleteIndex( index ) ) {

    // cout << print() << '\n';

    if ( index < bestIndex ) {
      bestIndex--;
    }
    
    // cout << "Inserted Schedule: ";

    insertVertexAtIndex( v, bestIndex );

    // cout << print() << '\n';

  }

  return 1;
}

// Split all parallel blocks containing different stars
//
// @Description Check each parallel block and split those that contain more
// than one star into multiple, sequential blocks.  In the end, all parallel
// blocks will be firing outputs on a single star, so a single firing of
// the star will suffice. <BR>
//
// By default, all vertices in the head of a partition are placed into a
// single parallel block (see addPartition).  This method splits them apart
// so the scheduler has an easier time. <BR>
//
// If a parallel block consists of [5 6 7 8 9], and 5, 6, and 9 are outputs of
// the same star, and 7 and 8 are outputs of a different star, this splits
// the schedule into [5 6] [7 8] [9]. <BR>
//
// The expectation is that when the schedule is generated, vertices on the
// same star will remain blocked together and this behavior will not
// happen.
void
SRRecursiveSchedule::splitParallelBlocks()
{
  SRStar * s;
  int v = 0;
  while ( v < mygraph->vertices() ) {

    int ps = parSize[v];
    int eb = v + ps;

    // cout << "Considering [";
    // for ( int k = v ; k < eb ; k++ ) cout << vertex[k] << ' ';
    // cout << "]\n";

    if ( ps > 1 ) {

      // A non-trivial parallel block -- break into parallel blocks
      // with identical stars

      while ( v < eb ) {
	s = mygraph->star(vertex[v]);
	int j = v + 1;

	// Find the next index with a star that doesn't match, or the
	// end of the parallel block, whichever comes sooner

	while ( j < eb && mygraph->star(vertex[j]) == s ) {
	  j++;
	}
	parSize[v] = j - v;
	if ( partSize[v] == 0 ) {
	  partSize[v] = j - v;
	}
	// cout << "Setting parSize[" << v << "] to " << j-v << '\n';
	v = j;
      }

      // cout << print() << '\n';

    } else {
      // A trivial parallel block -- just move on
      v++;
    }
  }

}

// Remove an index from the schedule
//
// @Description This leaves the schedule in a "broken" form that must
// be corrected with a matching insertIndex.
int
SRRecursiveSchedule::deleteIndex( int index /* Index to be deleted */)
{

  if ( headSize[index] == 1 ) {
    // Attempting to delete the only index in a head -- fail
    return 0;
  }

  int oldVertex = vertex[index];

  for ( int i = 0 ; i < index ; i++ ) {

    // If the index falls within the partition, head, or parallel block,
    // decrease each of these as appropriate

    if ( index < i + partSize[i] ) partSize[i]--;
    if ( index < i + headSize[i] ) headSize[i]--;
    if ( index < i + parSize[i] ) parSize[i]--;
  }

  // i is now at the index -- remove the node as necessary

  if ( i < mygraph->vertices()-1 ) {

    // i is not at the end of the schedule

    vertex[i] = vertex[i+1];
    
    if ( parSize[i] > 1 ) {
      // The node was at the beginning of a larger parallel block -- fix it
      parSize[i]--;
    } else {
      parSize[i] = parSize[i+1];
    }

    if ( partSize[i] > 1 ) {
      // The node was at the beginning of a larger partition -- fix it
      partSize[i]--;
    } else {
      partSize[i] = partSize[i+1];
    }

    if ( headSize[i] > 1 ) {
      // The node was at the beginning of a larger head -- fix it
      headSize[i]--;
    } else {
      headSize[i] = headSize[i+1];
      repCount[i] = repCount[i+1];
    }

  }

  i++;
  
  // Move the remaining schedule back one

  for ( ; i < mygraph->vertices() - 1; i++ ) {
    vertex[i] = vertex[i+1];
    partSize[i] = partSize[i+1];
    headSize[i] = headSize[i+1];
    parSize[i] = parSize[i+1];
    repCount[i] = repCount[i+1];
  }

  // Tack the vertex onto the end

  i = mygraph->vertices() - 1;
  vertex[i] = oldVertex;
  partSize[i] = 1;
  headSize[i] = 0;
  parSize[i] = 1;
  repCount[i] = 0;

  return 1;

}

// Insert the given vertex before the given index
int
SRRecursiveSchedule::insertVertexAtIndex(
  int v /* vertex to be inserted */,
  int index /* insert before this index */ )
{

  int i;

  for ( i = 0 ; i < index ; i++ ) {
    // If the new index falls within the partition, head, or parallel block,
    // increase each of these as appropriate

    if ( index < i + partSize[i] ) partSize[i]++;
    if ( index < i + headSize[i] ) headSize[i]++;
    if ( index < i + parSize[i] ) parSize[i]++;

  }

  // make space for everything beyond the index

  for ( i = mygraph->vertices() ; --i > index ; ) {   
    vertex[i] = vertex[i-1];
    partSize[i] = partSize[i-1];
    headSize[i] = headSize[i-1];
    parSize[i] = parSize[i-1];
    repCount[i] = repCount[i-1];
  }

  // i is now at the index -- insert the vertex as necessary

  // If the index was previously a partition, head, or parallel block,
  // insert this vertex at the beginning.

  vertex[i+1] = vertex[i];
  vertex[i] = v;

  if ( partSize[i] > 0 ) {
    partSize[i]++;
    partSize[i+1] = 0;
  } else {
    partSize[i+1] = partSize[i];
    partSize[i] = 1;
  }

  if ( headSize[i] > 0 ) {
    headSize[i]++;
    headSize[i+1] = 0;
    repCount[i+1] = 0;
  } else {
    headSize[i+1] = headSize[i];
    headSize[i] = 0;
    repCount[i+1] = repCount[i];
  }

  if ( parSize[i] > 0 ) {
    parSize[i]++;
    parSize[i+1] = 0;
  } else {
    parSize[i+1] = parSize[i];
    parSize[i] = 1;
  }

  return 1;
}
