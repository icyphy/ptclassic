#ifndef _DFCluster_h
#define _DFCluster_h 1
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
#include "DataFlowStar.h"
#include "DFPortHole.h"

class DFClusterPort : public DFPortHole, public ClusterPort{
public:
    DFClusterPort(PortHole* master, Star* parent);
    /*virtual*/ int isItInput() const {
	return ClusterPort::isItInput();
    }
    /*virtual*/ int isItOutput() const {
	return ClusterPort::isItOutput();
    }
    /*virtual*/ ClusterPort* asClusterPort() { return this; }
};

class DFCluster : public DataFlowStar, public DFClusterBase{
public:
    /*virtual*/ int run();

    // Constructors
    DFCluster(const char* domain);

    /*virtual*/ PortHole* clonePort(PortHole* master, Star* parent);
    
    /*virtual*/ Cluster* newCluster(Block* master,const char* domain) const;

    /*virtual*/ Cluster* asCluster() { return this; }
    
    /*virtual*/ const Cluster* asCluster() const { return this; }
    
    /*virtual*/ int isSDFinContext() const {
	return isClusterSDFinContext();
    }

    // return my domain.  all clusters should redefine this method of block.
    /*virtual*/ const char* domain () const { return myDomain; }
};
#endif





