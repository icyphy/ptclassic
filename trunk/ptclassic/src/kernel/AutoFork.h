/**************************************************************************
Version identification:
$Id$

Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
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
							COPYRIGHTENDKEY

 Programmer: J. Buck
 Date of creation: 2/13/92

 This used to be part of AutoForkNode.  It is now a separate class,
 to make it easier to define other types of auto-forking nodes
 (such as AsmForkNode for assembly language stars).

*************************************************************************/

#ifndef _AutoFork_h
#define _AutoFork_h 1

#ifdef __GNUG__
#pragma interface
#endif

class Geodesic;
class Block;
class MultiPortHole;
class GenericPort;
class PortHole;

class AutoFork {
public:
	AutoFork(Geodesic& g) : geo(g), forkStar(0), forkOutput(0) {}
	~AutoFork();
	// attach source or destination to the autofork
	PortHole* setSource(GenericPort&, int delay = 0);
	PortHole* setDest(GenericPort&, int alwaysFork = 0);
private:
	// associated geodesic
	Geodesic& geo;
	// associated automatically inserted fork star
	Block* forkStar;
	// points to output multiport of fork
	MultiPortHole* forkOutput;
	// cruft used for generating names for autoforks
	static int nF;
	static const char* autoForkName();
};
#endif
