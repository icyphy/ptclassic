/* -*- C++ -*-
Version $Id$ 

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

class Galaxy;
class SRStar;
class OutSRPort;
class StringList;

/***********************************************************************

  The dependency graph for an SR galaxy

  @Description This graph has one vertex per star output in the galaxy.
  The edges represent dependencies, which are usually the communication
  channels.

 **********************************************************************/

class SRDependencyGraph {
public:

  SRDependencyGraph( Galaxy & );
  ~SRDependencyGraph();

  // Return the number of vertices
  int vertices() const { return numvertices; }

  // Return the number of forward edges from a vertex
  int forwardEdges(int v) const { return fEdgeCount[v]; }

  // Return the destination vertex of a forward edge
  int destination(int v, int e) const { return fEdge[v][e]; }

  // Return the number of backward edges into a vertex
  int backwardEdges(int v) const { return bEdgeCount[v]; }

  // Return the source vertex of a backward edge
  int source(int v, int e) const { return bEdge[v][e]; }

  // Return the star of a vertex
  SRStar * star(int v) const { return stars[v]; }

  StringList displayGraph() const;

private:

  // Galaxy from which this graph was derived
  Galaxy * mygalaxy;

  // Number of vertices in the graph
  int numvertices;

  // Array of pointers to the star of each vertex
  //
  // @Description These are the stars at the top-level, so they may
  // include galaxies.
  SRStar ** stars;

  // Array of pointers to the port of each vertex
  OutSRPort ** ports;

  // Number of forward edges on each vertex
  int * fEdgeCount;

  // Number of backward edges on each vertex
  int * bEdgeCount;

  // Array of forward edge lists
  int ** fEdge;

  // Array of backward edge lists
  int ** bEdge;

  int vertexOfStarPort( SRStar *, OutSRPort * ) const;

};

