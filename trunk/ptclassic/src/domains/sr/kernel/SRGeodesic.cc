/* -*- c++ -*- */
 
/*  Version $Id$
 
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
    Created:    25 October 1996

*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRGeodesic.h"
#include "SRPortHole.h"
#include "Block.h"
#include <stream.h>
#include <assert.h>


// Construct an unconnected Geodesic
SRGeodesic::SRGeodesic()
{}

// Destroy a Geodesic, disconnecting all connected PortHoles
SRGeodesic::~SRGeodesic()
{

  if ( originatingPort ) {
    originatingPort->disconnect(0);
    originatingPort = NULL;
  }

  ListIter nextReceiver( receivers );
  for ( int size = receivers.size() ; --size >= 0 ; ) {
    SRPortHole * receiver = (SRPortHole *) nextReceiver++;
    if ( receiver ) {
      receiver->disconnect(0);
    }
  }

  // Clear these references even though the SR domain doesn't use them
  destinationPort = NULL;
}

// Set this Geodesic's driver (node-style)
//
// @Description Used for node-style connections.  Calls newConnection before
// calling setOldSourcePort

PortHole * SRGeodesic::setSourcePort( GenericPort & p, int d, const char * )
{
  return setOldSourcePort( p.newConnection(), d );
}

// Set this Geodesic's driver (point-to-point)
//
// @Description Used for both point-to-point and node-style connections.
// Disconnect the old driver if necessary.

PortHole * SRGeodesic::setOldSourcePort( GenericPort & p, int, const char * )
{
  //  cout << "SRGeodesic::setSourcePort called on " << name() << " to "
  //       << p.parent()->name() << " " << p.name() << "\n";

  if ( originatingPort ) {
    originatingPort->disconnect(0);
  }

  originatingPort = (PortHole *) &p;

  ((SRPortHole &) p).setGeodesic(this);

  return (PortHole *) &p;  
}

// Add a receiver to the list of receivers in this Geodesic
//
// @Description Used exclusively for node-style connections.  Calls
// newConnection before calling setOldDestPort

PortHole * SRGeodesic::setDestPort( GenericPort & p )
{
  return setOldDestPort( p.newConnection() );
}


// Add a receiver to the list of receivers in this Geodesic
//
// @Description Used for both node and point-to-point connections.

PortHole * SRGeodesic::setOldDestPort( GenericPort & p )
{
  //  cout << "SRGeodesic::setDestPort called on " << name() << " to "
  //       << p.parent()->name() << " " << p.name() << "\n";

  // Add this destination port to the list of receivers
  receivers.append( &p );

  // Normally, portHoleConnect() would do this, but that assumes
  // point-to-point connections
  ((SRPortHole &) p).setGeodesic(this);

  return (PortHole *) &p;

}
