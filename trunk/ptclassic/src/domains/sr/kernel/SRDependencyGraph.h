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

  // Return the destination vertex of an edge
  int destination(int v, int e) const { return fEdge[v][e]; }

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
  //
  // @Description These are the ports on the lowest-level stars.
  // Calling undoAliases() on these gives the ports actually connected
  // to the stars in stars.  Use doAliases() on those to get back.
  OutSRPort ** ports;

  // Forward edge counts
  int * fEdgeCount;

  // Array of forward edge lists
  int ** fEdge;

  int vertexOfStarPort( SRStar *, OutSRPort * ) const;

};

