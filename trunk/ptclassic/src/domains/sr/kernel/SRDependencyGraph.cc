static const char file_id[] = "SRRecursiveScheduler.cc";

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

#include "SRDependencyGraph.h"
#include "SRStar.h"
#include "SRPortHole.h"
#include "GalIter.h"
#include "GraphUtils.h"
#include <assert.h>
// #include <stream.h>

// Build a dependency graph for a galaxy
//
// @Description Make a vertex for each star output, an edge going from each
// output to every output it drives.
//
// <P>This makes multiple passes through the stars in a galaxy
// to allow all the arrays to be allocated statically.
//
// <P> Stars with no outputs are given a vertex with an empty
// port pointer.
//
// <P> No edges are added for independent input ports.

SRDependencyGraph::SRDependencyGraph( Galaxy & g )
{
  int vs, vd;
  InSRPort * ip;
  OutSRPort * op, * fop;
  SRStar * s;
  SRStar * fs;

  mygalaxy = &g;

  GalStarIter nextStar(g);

  // Count the number of vertices by summing the number of outputs on each
  // star.  Stars with no outputs are treated as if they had one.

  numvertices = 0;

  // nextStar.reset();

  while ( (s = (SRStar *)(nextStar++)) != NULL ) {
    BlockOutputIter nextOutput(*s);
    nextOutput++;
    do {
      numvertices++;
    } while ( nextOutput++ );
  }

  // Record the star and port for each vertex

  stars = new SRStar * [ numvertices ];
  ports = new OutSRPort * [ numvertices ];

  vs = 0;
  nextStar.reset();

  while ( (s = (SRStar *)(nextStar++)) != NULL ) {
    BlockOutputIter nextOutput(*s);

    // This odd iteration structure registers one vertex for a star
    // with no outputs (i.e., the initial nextOutput++ returns NULL)

    op = (OutSRPort *)(nextOutput++);
    do {
      stars[vs] = s;
      ports[vs] = op;
      vs++;
      op = (OutSRPort *)(nextOutput++);
    } while ( op != NULL );
  }

  // We'd better have gone through exactly all the vertices
  assert( vs == numvertices );

  // Calculate the edge counts for each vertex by iterating over all
  // outputs.  At each output, find its vertex (vd), then look at all the
  // inputs driving this star, find their vertices, and add the edge counts.

  // Again, if there are no outputs on the star, the first op is NULL and
  // treated normally.
  
  fEdgeCount = new int[numvertices];
  bEdgeCount = new int[numvertices];
  for ( vs = numvertices ; --vs >= 0 ; ) {
    fEdgeCount[vs] = bEdgeCount[vs] = 0;
  }

  nextStar.reset();
  while ( (s = (SRStar *)(nextStar++)) != NULL ) {
    BlockOutputIter nextOutput(*s);
    op = (OutSRPort *)(nextOutput++);
    do {
      vd = vertexOfStarPort( s, op );

      // We should be able to find this (destination) vertex
      assert( vd >= 0 );

      BlockInputIter nextInput(*s);
      while ( (ip = (InSRPort *)(nextInput++)) != NULL ) {
	if ( !(ip->isItIndependent()) ) {
	  if ( (fop = (OutSRPort *) (ip->far())) != NULL ) {
	    fs = (SRStar *) fop->parent();
	    vs = vertexOfStarPort( fs, fop );

	    // cout << "Found something connected to vertex " << vs << "\n";
	  
	    // We should be able to find the vertex driving this input
	    assert( vs >= 0 );

	    // Record this edge

	    if ( vs != vd ) {
	      fEdgeCount[vs]++;
	      bEdgeCount[vd]++;
	    }

	  } else {
	    //	  cout << "input " << ip->name() << " on " << s->name()
	    //	       << " has no far port\n";
	  }
	}
      }
      op = (OutSRPort *)(nextOutput++);
    } while ( op != NULL );

  }

  // Form the edge lists

  int * fedgenumber = new int[numvertices];
  int * bedgenumber = new int[numvertices];
  fEdge = new int *[numvertices];
  bEdge = new int *[numvertices];
  for ( vs = numvertices ; --vs >= 0 ; ) {
    if ( fEdgeCount[vs] != 0 ) {
      fEdge[vs] = new int[ fEdgeCount[vs] ];
    } else {
      fEdge[vs] = NULL;
    }
    if ( bEdgeCount[vs] != 0 ) {
      bEdge[vs] = new int[ bEdgeCount[vs] ];
    } else {
      bEdge[vs] = NULL;
    }
    fedgenumber[vs] = 0;
    bedgenumber[vs] = 0;
  }

  nextStar.reset();
  while ( (s = (SRStar *)(nextStar++)) != NULL ) {
    BlockOutputIter nextOutput(*s);
    op = (OutSRPort *)(nextOutput++);
    do {
      vd = vertexOfStarPort(s, op );

      // We should have been able to find the output vertex
      assert(vd >= 0);

      BlockInputIter nextInput(*s);
      while ( (ip = (InSRPort *)(nextInput++)) != NULL ) {
	if ( !(ip->isItIndependent()) ) {
	  if ( (fop = (OutSRPort *) (ip->far())) != NULL ) {
	    fs = (SRStar * ) fop->parent();
	    vs = vertexOfStarPort( fs, fop );

	    // We should have been able to find the vertex driving this input
	    assert( vs >= 0 );
	
	    // Add the forward and backward edge to the lists

	    if ( vs != vd ) {
	      fEdge[vs][ fedgenumber[vs]++ ] = vd;
	      bEdge[vd][ bedgenumber[vd]++ ] = vs;
	    }

	  }
	}
      }
      op = (OutSRPort *)(nextOutput++);
    } while ( op != NULL );
  }

  // We should have entered all the vertices we found previously

  for ( vs = numvertices ; --vs >= 0 ; ) {
    assert( fedgenumber[vs] == fEdgeCount[vs] );
    assert( bedgenumber[vs] == bEdgeCount[vs] );
  }

  delete [] fedgenumber;
  delete [] bedgenumber;
 
}

// Destroy the dependency graph

SRDependencyGraph::~SRDependencyGraph()
{

  delete [] stars;
  delete [] ports;
  delete [] fEdgeCount;
  delete [] bEdgeCount;
  for ( int i = numvertices ; --i >= 0 ; ) {
    delete [] fEdge[i];
    delete [] bEdge[i];
  }
  delete [] fEdge;
  delete [] bEdge;

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
    out << v << ": " << stars[v]->name() << " ";
    if ( ports[v] ) {
      out << ports[v]->name() << "\n  -> ";
    } else {
      out << "(fictitious)\n  -> ";
    }
    for ( int e = 0 ; e < fEdgeCount[v] ; e++ ) {
      out << fEdge[v][e] << " ";
    }
    out << "\n  <- ";
    for ( e = 0 ; e < bEdgeCount[v] ; e++ ) {
      out << bEdge[v][e] << " ";
    }
    out << "\n";
  }
  
  return out;

}
