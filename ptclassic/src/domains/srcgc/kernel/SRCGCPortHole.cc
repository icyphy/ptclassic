/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCPortHole.cc"
 *                                    created: 9/03/98 16:40:43 
 *                                last update: 13/05/98 9:21:41 
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
static const char file_id[] = "SRCGCPortHole.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "ConstIters.h"
#include "CGUtilities.h"
#include "SRCGCPortHole.h"
#include "SRCGCGeodesic.h"
#include "SRCGCStar.h"
#include "Error.h"
#include "SimControl.h"

/* Already defined in CGCPortHole.cc                */
/* extern const Attribute P_GLOBAL = {PB_GLOBAL,0}; */
/* extern const Attribute P_LOCAL = {0,PB_GLOBAL};  */


SRCGCPortHole :: ~SRCGCPortHole() {
  LOG_DEL; delete [] bufName;
  SRCGCPortHole::disconnect(1);
}

void SRCGCPortHole :: initialize() {
  CGPortHole :: initialize();

  // member initialize. same values as constructor.
  maxBuf = 1;
  manualFlag = FALSE;
  hasStaticBuf = TRUE;
  asLinearBuf = TRUE;
  converted = FALSE;
  delete [] bufName;
  bufName = 0;
  manualOffset = 0;      
}


// Connect this port to another
//
// @Description This is used for point-to-point connections and
// will either connect to the SRCGCGeodesic of either port, or create a new one
// if needed.  Calls SRCGCGeodesic::setOldDestPort and
// SRCGCGeodesic::setOldSourcePort to make the connection because it
// calls newConnection.
void SRCGCPortHole::connect(GenericPort& destination,
                            int, const char *) {
  SRCGCPortHole & nearPort = (SRCGCPortHole &) newConnection();
  SRCGCPortHole & farPort = (SRCGCPortHole &) destination.newConnection();

  enum { connect_both, connect_near, connect_far } c;

  Geodesic * g = nearPort.PortHole::geo();
  if ( g ) {
    // The near port has a geodesic, so connect the far port to it
    c = connect_far;
  } else {
    g = farPort.PortHole::geo();
    if ( g ) {
      // The destination has a geodesic, so connect the source to it
      nearPort.myGeodesic = g;
      c = connect_near;
    } else {
      // Nobody has a geodesic -- create one and connect both to that
      LOG_NEW; g = new SRCGCGeodesic;
      c = connect_both;      
    }
  }
  if ( c != connect_near ) {
    farPort.myGeodesic = g;
    if ( farPort.isItInput() ) {
     ((SRCGCGeodesic *) g)->setOldDestPort( farPort );
    } else {
      ((SRCGCGeodesic *) g)->setSourcePort( farPort, 0, 0 );
    }
  }
  if ( c != connect_far ) {
    nearPort.myGeodesic = g;
    if ( nearPort.isItInput() ) {
      ((SRCGCGeodesic *) g)->setOldDestPort( nearPort );
    } else {
     ((SRCGCGeodesic *) g)->setOldSourcePort( nearPort, 0, 0 );
    }
  }
}

// Disconnect this PortHole, removing it from its Geodesic, if any
//
// @Disconnect This overrides PortHole::disconnect, which expects
// farSidePort to always be correct.  With the SR domain's multiple
// connections, this is impossible.
void SRCGCPortHole::disconnect(int delGeo) {
  if ( myGeodesic ) {
    myGeodesic->disconnect(*this);
    if ( delGeo ) {
      LOG_DEL; delete myGeodesic;
    }
    myGeodesic = NULL;
  }
  farSidePort = NULL;
}


// Return the PortHole that drives this one
//
// @Description This goes through the geodesic to find the source port,
// rather than using the farSidePort field.
PortHole * SRCGCPortHole::far() const {
  if ( myGeodesic ) {
    return myGeodesic->sourcePort();
  }
  return NULL;
}

// Set the geodesic of this port
//
// @Description This also sets the farSidePort field, which is equal to
// the driver for inputs, and equal to one of the receivers (the first in the
// list) for an output.  This nonsense is for the type resolution system.
void SRCGCPortHole::setGeodesic( Geodesic * g ) {
  myGeodesic = g;
  if ( (SRCGCPortHole *)(far()) != this ) {
    farSidePort = far();
  } else {
    farSidePort = ((SRCGCGeodesic *)g)->getReceiver();
  }
  if (farSidePort != NULL && farSidePort->far() ==NULL) {
    ((SRCGCPortHole *)farSidePort)->setGeodesic(g);
  }
}


void SRCGCPortHole::setFlags() {
  if (isItOutput() && (embedded() || embedding())) {
    asLinearBuf = TRUE;
    hasStaticBuf = TRUE;
    return;
  }
  if (bufSize() % numXfer() != 0)
    asLinearBuf = FALSE;
  
  // BufferSize might not be set - bufSize class error abort run
  if (SimControl::haltRequested())
    return;

  if (!bufSize()) {
      Error::abortRun(*this,
        " Buffer size has not been set.  Has the galaxy been scheduled?");
      return;
  }

  if ((numXfer() * parentReps()) % bufSize() != 0)
    hasStaticBuf = FALSE; 
}


    ////////////////////////////////////////
    // Buffer size determination routines
    ////////////////////////////////////////

// initialize the offset member. If there is no fork involved, output
// porthole start writing from offset 0, and the input porthole
// start reading from the (maxBuf - offset).
int SRCGCPortHole :: initOffset() {
  return TRUE;
}


SRCGCPortHole* SRCGCPortHole :: realFarPort() {
  SRCGCPortHole* p = (SRCGCPortHole*) far();
  return p;
}

// const version: hate this duplication.
const SRCGCPortHole* SRCGCPortHole :: realFarPort() const {
  const SRCGCPortHole* p = (const SRCGCPortHole*) far();
  return p;
}

void SRCGCPortHole :: setGeoName(const char* n) {
  delete [] bufName;
  if (myGeodesic == 0) bufName = savestring(n);
  else geo().setBufName(n);
}

const char* SRCGCPortHole :: getGeoName() const {
  if (bufName) return bufName;
  return geo().getBufName();
}


// Used by SRCGCPrinter star : 
// return the print format for the C function 'printf(...)' corresponding 
// to the type of the port.
const char* SRCGCPortHole :: getPrintFormat() const {
  DataType type = resolvedType();
    
  if (type == INT) {
    return "%d";
  } else if (type == STRING) {
    return "%s";
  } else return "%f";
}


// Dummy
int InSRCGCPort :: isItInput() const { return TRUE; }

void InSRCGCPort::initialize() {
  PortHole::initialize();

  isIndependent = FALSE;
}

int OutSRCGCPort :: isItOutput() const { return TRUE; }

// Identify the port as an input
int MultiInSRCGCPort::isItInput() const {
  return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiInSRCGCPort::newPort() {
  LOG_NEW; SRCGCPortHole& p = *new InSRCGCPort;
  return installPort(p);
}

// Input/output identification.
int MultiOutSRCGCPort::isItOutput() const {
  return TRUE;
}

// Add a new physical port to the MultiPortHole list.
PortHole& MultiOutSRCGCPort::newPort() {
  LOG_NEW; SRCGCPortHole& p = *new OutSRCGCPort;
  return installPort(p);
}

// Return a PortHole for a new connection.
//
// @Description This almost exactly replicates the behavior of
// MultiPortHole::newConnection() in the Kernel.  However, since
// the far() method is not virtual in the kernel, it incorrectly determines
// whether the port has a connection.  This casts the port to InSRPort, which
// uses the virtual far() method and gets it right.
PortHole & MultiInSRCGCPort::newConnection() {
  // first resolve aliases.
  GenericPort & real = realPort();

  // check for being aliased to a single porthole (is this possible?)
  if (!real.isItMulti())
    return (PortHole&)real;

  MultiInSRCGCPort & realmph = (MultiInSRCGCPort &) real;

  // find an unconnected porthole in the real port
  MPHIter next(realmph);
  InSRCGCPort * p;
  while ((p = (InSRCGCPort *)(next++)) != 0) {
    // do the right thing if a member of realmph is an alias.
    // I think this is not really possible, but play it safe.
    InSRCGCPort & realp = (InSRCGCPort &) (p->newConnection());
    // realp is disconnected iff it has no far()
    if (realp.far() == NULL)
      return realp;
    // We cannot just check far() on p since, if
    // it is an alias, its far will be null.
    // And we don't want to return an alias anyway.
  }
  
  // no disconnected ports, make a new one.
  // NOTE: newPort() on a GalMultiPort returns the new alias porthole,
  // so we have to resolve it to its real porthole.
  return newPort().newConnection();
}
