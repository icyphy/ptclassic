#ifndef _CGCluster_h
#define _CGCluster_h 1
#ifdef __GNUG__
#pragma interface
#endif

/*******************************************************************
 SCCS Version identification :
 $Id$

Copyright (c) 1990-%Q% The Regents of the University of California.
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

 Programmer : Jose Luis Pino
 Date of Creation : 6/10/90
	
********************************************************************/

#include "Cluster.h"
#include "DFClusterBase.h"
#include "CGStar.h"
#include "CGPortHole.h"

class CGClusterPort : public CGPortHole, public ClusterPort{
public:
    CGClusterPort(PortHole* master, Star* parent);
    /*virtual*/ int isItInput() const {
	return ClusterPort::isItInput();
    }
    /*virtual*/ int isItOutput() const {
	return ClusterPort::isItOutput();
    }
    /*virtual*/ ClusterPort* asClusterPort() { return this; }
};

class CGCluster : public CGStar, public DFClusterBase{
public:
    
    CGCluster(const char* domain);

    /*virtual*/ int run();

    /*virtual*/ int flattenGalaxy(Galaxy* galaxy) {
	return ! (int)galaxy->stateWithName("Scheduler");
    }

    /*virtual*/ void setMasterBlock(Block*,PortHole** = NULL);

    /*virtual*/ PortHole* clonePort(PortHole* master, Star* parent);
    
    /*virtual*/ Cluster* newCluster(Block* = NULL,const char* = NULL) const;

    /*virtual*/ Block* makeNew() const { return &newCluster()->star();}

    /*virtual*/ Block* clone() const { return cloneCluster(); }
    
    /*virtual*/ Cluster* asCluster() { return this; }
    
    /*virtual*/ const Cluster* asCluster() const { return this; }
    
    /*virtual*/ int isSDFinContext() const {
	return isClusterSDFinContext();
    }

    /*virtual*/ int myExecTime();
    
    // return my domain.  all clusters should redefine this method of block.
    /*virtual*/ const char* domain () const { return myDomain; }
};
#endif





