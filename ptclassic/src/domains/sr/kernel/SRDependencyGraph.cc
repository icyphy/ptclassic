static const char file_id[] = "SRRecursiveScheduler.cc";

/* Version $Id$

Copyright (c) 1996-%Q% The Regents of the University of California.
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

#include "SRDependencyGraph.h"
#include "SRStar.h"
#include "SRPortHole.h"
#include "GalIter.h"
#include "GraphUtils.h"
#include <assert.h>
#include <stream.h>

// Build a dependency graph for a galaxy
//
// @Description Make a vertex for each star output, an edge going from each
// output to every output it drives.
//
// <P>This makes multiple passes through the stars in a galaxy
// to allow all the arrays to be  allocated statically.

SRDependencyGraph::SRDependencyGraph( Galaxy & g )
{
  int vs, vd;
  InSRPort * ip;
  OutSRPort * op, * fop;
  SRStar * s;
  SRStar * fs;

  mygalaxy = &g;

  GalTopBlockIter nextStar(g);

  // Count the number of vertices by summing the number of outputs on each
  // star

  numvertices = 0;

  // nextStar.reset();
  while ( (s = (SRStar *)(nextStar++)) != NULL ) {
    BlockOutputIter nextOutput(*s);
    while ( nextOutput++ ) {
      numvertices++;
    }
  }

  // Record the star and port for each vertex

  stars = new SRStar * [ numvertices ];
  ports = new OutSRPort * [ numvertices ];

  vs = 0;
  nextStar.reset();  
  while ( (s = (SRStar *)(nextStar++)) != NULL ) {
    BlockOutputIter nextOutput(*s);
    while ( (op = (OutSRPort *)(nextOutput++)) != NULL ) {
      op = (OutSRPort *) op->doAliases();
      stars[vs] = s;
      ports[vs] = op;
      vs++;
    }
  }

  // We'd better have gone through exactly all the vertices
  assert( vs == numvertices );

  // Calculate the edge counts for each vertex
  
  fEdgeCount = new int[numvertices];
  for ( vs = numvertices ; --vs >= 0 ; ) {
    fEdgeCount[vs] = 0;
  }

  nextStar.reset();
  while ( (s = (SRStar *)(nextStar++)) != NULL ) {
    BlockOutputIter nextOutput(*s);
    while ( (op = (OutSRPort *)(nextOutput++)) != NULL ) {
      BlockInputIter nextInput(*s);
      while ( (ip = (InSRPort *)(nextInput++)) != NULL ) {
	ip = (InSRPort *) ip->doAliases();
	if ( (fop = (OutSRPort *) (ip->far())) != NULL ) {
	  fs = (SRStar *) fop->undoAliases()->parent();
	  vs = vertexOfStarPort( fs, fop );

	  cout << "Found something connected to vertex " << vs << "\n";
	  
	  // We should be able to find the vertex driving this
	  // input
	  assert( vs >= 0 );
	  
	  fEdgeCount[vs]++;
	} else {
	  cout << "input " << ip->name() << " on " << s->name()
	       << " has no far port\n";
	}
      }
    }
  }

  // Form the edge lists

  int * edgenumber = new int[numvertices];
  fEdge = new int *[numvertices];
  for ( vs = numvertices ; --vs >= 0 ; ) {
    if ( fEdgeCount[vs] != 0 ) {
      fEdge[vs] = new int[ fEdgeCount[vs] ];
    } else {
      fEdge[vs] = NULL;
    }
    edgenumber[vs] = 0;
  }

  nextStar.reset();
  while ( (s = (SRStar *)(nextStar++)) != NULL ) {
    BlockOutputIter nextOutput(*s);
    while ( (op = (OutSRPort *)(nextOutput++)) != NULL ) {
      op = (OutSRPort *) op->doAliases();
      vd = vertexOfStarPort(s, op );

      // We should have been able to find the output vertex
      assert(vd >= 0);

      BlockInputIter nextInput(*s);
      while ( (ip = (InSRPort *)(nextInput++)) != NULL ) {
	ip = (InSRPort *) ip->doAliases();
	if ( (fop = (OutSRPort *) (ip->far())) != NULL ) {
	  fs = (SRStar *) fop->undoAliases()->parent();
	  vs = vertexOfStarPort( fs, fop );

	  // We should have been able to find the vertex driving this input
	  assert( vs >= 0 );
	
	  fEdge[vs][ edgenumber[vs]++ ] = vd;
	}
      }
    }
  }

  // We should have entered all the vertices we think

  for ( vs = numvertices ; --vs >= 0 ; ) {
    assert( edgenumber[vs] == fEdgeCount[vs] );
  }

  delete [] edgenumber;
 
}

// Destroy the dependency graph

SRDependencyGraph::~SRDependencyGraph()
{

  delete [] stars;
  delete [] ports;
  delete [] fEdgeCount;
  for ( int i = numvertices ; --i >= 0 ; ) {
    delete [] fEdge[i];
  }
  delete [] fEdge;

}

// Return the vertex corresponding to the given star and port
//
// @Description Returns -1 if the vertex was not found

int SRDependencyGraph::vertexOfStarPort( SRStar * s, OutSRPort * p ) const
{
  for (int i = numvertices ; --i >= 0 ; ) {
    if (stars[i] == s && ports[i] == p ) {
      return i;
    }
  }

  return -1;

}

// Return a printable form of the graph

StringList SRDependencyGraph::displayGraph() const
{
  StringList out;

  out << "Vertices: " << numvertices << "\n";

  for ( int v = 0 ; v < numvertices ; v++ ) {
    out << v << ": " << stars[v]->name() << " " << ports[v]->undoAliases()->name() << "\n  ";
    for ( int e = 0 ; e < fEdgeCount[v] ; e++ ) {
      out << fEdge[v][e] << " ";
    }
    out << "\n";
  }
  
  return out;

}
