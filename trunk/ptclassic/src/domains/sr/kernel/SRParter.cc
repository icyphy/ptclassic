static const char file_id[] = "SRParter.cc";

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

#include "SRDependencyGraph.h"
#include "SRParter.h"
#include "Set.h"
#include "StringList.h"
#include <stream.h>
#include <assert.h>
#include <limits.h>

//SRPartType SRParter::parter = SRPartInOutT;
//SRPartType SRParter::parter = SRPartOneT;
//SRPartType SRParter::parter = SRPartExactT;
SRPartType SRParter::parter = SRPartSweepT;

int SRPart::ignoreSimpleFlag = 0;

// Empty virtual destructor
SRPart::~SRPart() {}

// Construct a new partitioner based on SRPartType and reset it
SRParter::SRParter( Set & s, SRDependencyGraph & g )
{

  switch ( parter ) {
  case SRPartOneT:
    mypart = new SRPartOne( s, g );
    break;
  case SRPartInOutT:
    mypart = new SRPartInOut( s, g );
    break;
  case SRPartExactT:
    mypart = new SRPartExact( s, g );
    break;
  case SRPartSweepT:
    mypart = new SRPartSweep( s, g );
    break;
  default:
    mypart = new SRPartOne( s, g );
    break;
  }

  mypart->init();

}

// Select the partitioning scheme by name
void SRParter::setParter( const char * p )
{
  if ( strcmp( p, "sweep" ) == 0 ) {
    parter = SRPartSweepT;
  } else if ( strcmp( p, "one" ) == 0 ) {
    parter = SRPartOneT;
  } else if ( strcmp( p, "exact" ) == 0 ) {
    parter = SRPartExactT;
  } else if ( strcmp( p, "inout" ) == 0 ) {
    parter = SRPartInOutT;
  } else {
    parter = SRPartSweepT;
  }
}

void SRPartOne::init()
{
  vertexIndex = -1;
}

Set * SRPartOne::next( int b /* Maximum size of the partition */ )
{
  if ( b >= 1 ) {

    // Find the next vertex in the set

    while ( vertexIndex < (partSet->size())-1 ) {
      if ( (*partSet)[++vertexIndex] ) {

	// Return a set containing only that vertex

	Set * s = new Set( partSet->size() );
	(*s) |= vertexIndex;
	return s;

      }
    }
  }

  return NULL;

}

// Find the vertex with minimum indegree or outdegree and build that vertex set
void SRPartInOut::init()
{
  int minInDegree = INT_MAX;
  int minInVertex = -1;
  int minOutDegree = INT_MAX;
  int minOutVertex = -1;

  SetIter next( *partSet );

  int v;

  while ( ( v = next++ ) >= 0 ) {
    int outdegree = 0;
    int e;
    for ( e = dgraph->forwardEdges(v) ; --e >= 0 ; ) {
      if ( (*partSet)[dgraph->destination(v,e)] ) {
	outdegree++;
      }
    }

    int indegree = 0;
    for ( e = dgraph->backwardEdges(v) ; --e >= 0 ; ) {
      if ( (*partSet)[dgraph->source(v,e)] ) {
	indegree++;
      }
    }

    if ( outdegree < minOutDegree ) {
      minOutDegree = dgraph->forwardEdges(v);
      minOutVertex = v;
    }
    if ( outdegree < minInDegree ) {
      minInDegree = dgraph->backwardEdges(v);
      minInVertex = v;
    }
  }

  minset = new Set( dgraph->vertices() );

  if ( minInDegree < minOutDegree ) {
    for ( int i = dgraph->backwardEdges(minOutVertex) ; --i >= 0 ; ) {
      if ( (*partSet)[dgraph->source(minOutVertex,i)] ) {
	(*minset) |= dgraph->source(minOutVertex,i);
      }
    }
  } else {
    for ( int i = dgraph->forwardEdges(minInVertex) ; --i >= 0 ; ) {
      if ( (*partSet)[dgraph->destination(minInVertex,i)] ) {
	(*minset) |= dgraph->destination(minInVertex,i);
      }
    }
  }

}

// Return the partition
Set * SRPartInOut::next(int bound)
{
  if ( minset != NULL && minset->cardinality() <= bound ) {
    Set * out = minset;
    minset = NULL;
    return out;
  }

  delete minset;
  minset = NULL;
  return NULL;
}

SRPartInOut::~SRPartInOut()
{
  delete minset;
}

// Set up all the index arrays
void
SRPartExact::init()
{
  maxpart = partSet->cardinality();
  vertex = new int [ maxpart ];
  vindex = new int [ maxpart ];

  // Fill the vertex set with the vertices in the subgraph

  SetIter next( *partSet );
  int i = 0;
  int v;
  while ( ( v = next++ ) >= 0 ) {
    vertex[i++] = v;
  }

  vindex[0] = 0;
  partsize = 1;

}

SRPartExact::~SRPartExact()
{
  delete [] vertex;
  delete [] vindex;
}


// Return all sets of size 1, size 2, etc.
//
// @Description Uses the vindex array to count in the following way, e.g.,
// <PRE>
//   0
//   1
//   2
//  10
//  20
//  21
// 321
// </PRE>

Set *
SRPartExact::next( int bound )
{
  int i;

  // If the bound is less than what we are currently generating,
  // there is nothing more to do since we will only generate larger
  // partitions

  if ( bound < partsize || partsize > maxpart ) {
    return NULL;
  }

  // cout << "maxpart is " << maxpart << "\n";
  // cout << "initial vindex has ";
  // for ( i = 0 ; i < partsize ; i ++ ) {
  //  cout << vindex[i] << " ";
  // }
  // cout << "\n";

  // Fill the set with the vertices indexed by vindex

  Set * s = new Set( partSet->size() );

  for ( i = partsize ; --i >= 0 ; ) {
    (*s) |= vertex[vindex[i]];
  }

  // cout << "Part: " << s->print() << "\n";

  int current = partsize - 1;

  do {

    // Increase the current index and put those after it in sequence

    vindex[current]++;
    for ( i = current+1 ; i < partsize ; i++) {
      vindex[i] = vindex[i-1]+1;
    }
    if ( vindex[partsize-1] >= maxpart ) {
      current--;
    }

  } while ( vindex[partsize-1] >= maxpart && current >= 0 );

  if ( current < 0 ) {
    
    // If we ran off the edge, increase the partsize and start over

    partsize++;
    if ( partsize < maxpart ) {
      for ( i = partsize ; --i >= 0 ; ) {
	vindex[i] = i;
      }
    }
  }

  assert( partsize <= maxpart );
  if ( partsize < maxpart ) {
    assert( vindex[partsize-1] < maxpart );
  }

  // cout << "vindex has ";
  // for ( i = 0 ; i < partsize ; i ++ ) {
  //   cout << vindex[i] << " ";
  // }
  // cout << "\n";

  return s;

}

void SRPartSweep::init()
{
  sgIter = new SetIter( *partSet );
  kernel = new Set( partSet->size() );

  // Start growing the kernel from the first vertex in the partSet

  (*kernel) |= (*sgIter)++;

  done = 0;
}

SRPartSweep::~SRPartSweep()
{
  delete sgIter;
  delete kernel;

}

Set * SRPartSweep::next( int bound )
{
  if ( !done ) {

    Set * theBorder = NULL;

    int borderSize;

    do {

      // cout << "With kernel " << kernel->print() << ",\n";
      delete theBorder;
      theBorder = border();
      borderSize = theBorder->cardinality();

      // cout << " the border is " << theBorder->print() << "\n";

      int bestVertex = -1;
      int bestCost = INT_MAX;

      // Compute the cost of adding each border vertex.
      // This is the number of distinct vertices leading out of the
      // vertex that are in the subgraph but not in the kernel.

      int borderv;
      SetIter nextBorder( *theBorder );
      while ( (borderv = nextBorder++) >= 0 ) {
	Set outgoingVertices( partSet->size() );
	for ( int e = dgraph->forwardEdges(borderv) ; --e >= 0 ; ) {	 
	  outgoingVertices |= dgraph->destination(borderv,e);
	}
	outgoingVertices &= *partSet;
	outgoingVertices &= *theBorder;

	int c = outgoingVertices.cardinality();
	if ( c < bestCost ) {
	  bestCost = c;
	  bestVertex = borderv;
	}
      }

      if ( bestVertex == -1 ) {

	// Clear the kernel set and add the next vertex, if there is one

	(*kernel) -= *kernel;

	int v;
        v = (*sgIter)++;
	// cout << "Starting new kernel with " << v << "\n";
	if ( v >= 0 ) {
	  (*kernel) |= v;
	} else {
	  done = 1;
	}

      } else {

	(*kernel) |= bestVertex;

      }
      
    } while ( !done && borderSize > bound );
    
    if ( borderSize > 0 && borderSize <= bound ) {
      return theBorder;
    } else {
      delete theBorder;
    }

  }

  return NULL;

}

// Return the border of the kernel
//
// @Description This is the set of vertices with incoming edges from the
// kernel that are in the subgraph but not in the kernel.
Set * SRPartSweep::border()
{
  Set * border = new Set( partSet->size() );

  SetIter next(*kernel);
  int kv;
  while ( (kv = next++) >= 0 ) {
    for (int e = dgraph->forwardEdges(kv) ; --e >= 0 ; ) {
      *border |= dgraph->destination(kv,e);
    }
  }
  *border &= *partSet;
  *border -= *kernel;

  return border;
}


