/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCWormhole.cc"
 *                                    created: 9/03/98 15:33:38 
 *                                last update: 13/05/98 9:22:00 
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
static const char file_id[] = "SRCGCWormhole.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCWormhole.h"
#include "SRCGCRecursiveScheduler.h"

/********************************************************************
 *  class SRCGCWormhole methods
 ********************************************************************/
// Constructor
SRCGCWormhole::SRCGCWormhole(Galaxy& g, Target* t)
             : CGWormBase(*this,g,t) {
  buildEventHorizons();
}

SRCGCWormhole::~SRCGCWormhole() { freeContents(); }

// cloner -- clone the inside and make a new wormhole from that.
Block* SRCGCWormhole::clone() const {
  LOG_NEW;
  return new SRCGCWormhole(gal.clone()->asGalaxy(), myTarget()->cloneTarget());
}

Wormhole* SRCGCWormhole::asWormhole() { return this; }

void SRCGCWormhole::go() {
  scheduler()->compileRun();
}

/**************************************************************************
 *  methods for SRCGCtoUniversal
 **************************************************************************/

void SRCGCtoUniversal::receiveData () {
  // 1. get data
  getData();

  // Check it is an input or output.
  // BUG: g++ 2.2.2 is screwup up the vtbl for fns inherited
  // from two different baseclasses.
  if (SRCGCtoUniversal::isItOutput()) {
    // 2. annul increment of currentTime at the end of run.
    SRCGCRecursiveScheduler* sched = (SRCGCRecursiveScheduler*) innerSched();
    timeMark = sched->now() - sched->schedulePeriod;
  }

  // transfer Data
  transferData();
}

void SRCGCtoUniversal::initialize() {
  PortHole::initialize();
  ToEventHorizon::initialize();   
}

int SRCGCtoUniversal::isItInput() const { return EventHorizon::isItInput(); }
int SRCGCtoUniversal::isItOutput() const {return EventHorizon:: isItOutput();}

EventHorizon* SRCGCtoUniversal::asEH() { return this; }

// Return the number of firings
int SRCGCtoUniversal::onlyOne() const {
  return TRUE;
}
  
/**************************************************************************
 *  methods for SRCGCfromUniversal
 **************************************************************************/

void SRCGCfromUniversal::sendData () {
  // 1. transfer data
  transferData();

  if (tokenNew) {
    // 2. put data
    putData();
  } 
  // no timeMark business since SRCGC is "untimed" domain.
}

// wait until has enough number of tokens to fire the inside star
// (#repetitions) times from SRCGC scheduler.
int SRCGCfromUniversal::ready() {
  DFPortHole *pFar = (DFPortHole*)far();
  int target = pFar->numXfer() * pFar->parentReps();
  if (numTokens() >= target) {
    // yes, enough tokens
    return TRUE;
  }
  return FALSE;
}

void SRCGCfromUniversal::initialize() {
  PortHole::initialize();
  FromEventHorizon::initialize();
}

int SRCGCfromUniversal::isItInput() const {return EventHorizon::isItInput();}
int SRCGCfromUniversal::isItOutput() const {return EventHorizon::isItOutput();}

EventHorizon* SRCGCfromUniversal::asEH() { return this; }

// Return the number of firings
int SRCGCfromUniversal::onlyOne() const {
  return TRUE;
}
