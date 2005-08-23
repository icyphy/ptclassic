/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCGeodesic.cc"
 *                                    created: 9/03/98 15:05:24 
 *                                last update: 13/05/98 9:21:25 
 *  Author: Vincent Legrand, Mathilde Roger, Frédéric Boulanger
 *  E-mail: Frederic.Boulanger@supelec.fr
 *    mail: Supélec - Service Informatique
 *          Plateau de Moulon, F-91192 Gif-sur-Yvette cedex
 *     www: http://wwwsi.supelec.fr/
 *  
 *  Thomson: Xavier Warzee <XAVIER.W.X.WARZEE@tco.thomson.fr>
 *  
 * Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
 * All rights reserved.
 * 
 * Permission is hereby granted, without written agreement and without
 * license or royalty fees, to use, copy, modify, and distribute this
 * software and its documentation for any purpose, provided that the
 * above copyright notice and the following two paragraphs appear in all
 * copies of this software.
 * 
 * IN NO EVENT SHALL SUPELEC OR THOMSON-CSF OPTRONIQUE BE LIABLE TO ANY PARTY
 * FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
 * ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
 * SUPELEC OR THOMSON-CSF OPTRONIQUE HAS BEEN ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 * 
 * SUPELEC AND THOMSON-CSF OPTRONIQUE SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
 * PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND SUPELEC OR THOMSON-CSF
 * OPTRONIQUE HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
 * ENHANCEMENTS, OR MODIFICATIONS.
 * 
 *  See header file for further information
 * ###################################################################
 */
static const char file_id[] = "SRCGCGeodesic.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCGeodesic.h"
#include "SRCGCPortHole.h"
#include "Error.h"

ISA_FUNC(SRCGCGeodesic,CGGeodesic);

// Destroy a Geodesic, disconnecting all connected PortHoles
SRCGCGeodesic::~SRCGCGeodesic() {
  SequentialList rec;

  if ( originatingPort ) {
    originatingPort->disconnect(0);
  }  

  // Copy the list of receivers onto a new list

  ListIter nextReceiver( receivers );
  
  int size;
  for ( size = receivers.size() ; --size >= 0 ; ) {
    rec.append((Pointer) nextReceiver++);
  }

  // Disconnect each one (this empties the receivers list)

  ListIter nextRec( rec );
  for ( size = rec.size() ; --size >= 0 ; ) {
    SRCGCPortHole * receiver = (SRCGCPortHole *) nextRec++;
    if ( receiver ) {
      receiver->disconnect(0);
    }    
  }
  
  LOG_DEL; delete [] bufName;
}

// Disconnect a PortHole from this Geodesic
//
// @Description Does not affect the PortHole itself.
int SRCGCGeodesic::disconnect( PortHole & p ) {
  if ( &p == originatingPort ) {
    originatingPort = NULL;
    return TRUE;
  }

  return receivers.remove( &p );
}

// Return the first receiver in the list, if any
PortHole * SRCGCGeodesic::getReceiver() {
  ListIter nextReceiver( receivers );
  return (PortHole *) nextReceiver++;
}

// Set this Geodesic's driver (node-style)
//
// @Description Used for node-style connections.  Calls newConnection before
// calling setOldSourcePort
PortHole * SRCGCGeodesic::setSourcePort( GenericPort & p, int d, const char * ) {
  return setOldSourcePort( p.newConnection(), d );
}

// Set this Geodesic's driver (point-to-point)
//
// @Description Used for both point-to-point and node-style connections.
// Disconnect the old driver if necessary.
PortHole * SRCGCGeodesic::setOldSourcePort( GenericPort & p, int, const char * ) {
  if ( originatingPort ) {
    originatingPort->disconnect(0);
  }

  originatingPort = (PortHole *) &p;

  ((SRCGCPortHole &) p).setGeodesic(this);

  return (PortHole *) &p;  
}

// Add a receiver to the list of receivers in this Geodesic
//
// @Description Used exclusively for node-style connections.  Calls
// newConnection before calling setOldDestPort
PortHole * SRCGCGeodesic::setDestPort( GenericPort & p ) {
  return setOldDestPort( p.newConnection() );
}


// Add a receiver to the list of receivers in this Geodesic
//
// @Description Used for both node and point-to-point connections.
PortHole * SRCGCGeodesic::setOldDestPort( GenericPort & p ) {
  // Add this destination port to the list of receivers
  receivers.append( &p );

  // Normally, portHoleConnect() would do this, but that assumes
  // point-to-point connections
  ((SRCGCPortHole &) p).setGeodesic(this);

  return (PortHole *) &p;
}

// Say that this Geodesic is persistent
//
// @Description This is to ensure PortHole::disconnect() doesn't destroy
// this geodesic just because one PortHole disconnects from it.
int SRCGCGeodesic::isItPersistent() const {
  return TRUE;
}


// Return the name assigned to the buffer.
// If I am a fork destination, my buffer name is that of the source.
char* SRCGCGeodesic::getBufName() const {
  const SRCGCPortHole* p = (const SRCGCPortHole*)src();
  return p ? p->geo().getBufName() : bufName;
}

// allocation control heuristic
double SRCGCGeodesic :: wasteFactor() const { return myWaste; }
