/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCRecursiveScheduler.cc"
 *                                    created: 9/03/98 16:41:11 
 *                                last update: 13/05/98 9:21:45 
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
static const char file_id[] = "SRCGCRecursiveScheduler.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCStar.h"
#include "SRCGCRecursiveScheduler.h"
#include "SRRecSchedule.h"
#include "GalIter.h"
#include "Galaxy.h"

extern const char SRCGCdomainName[];

// Return the name of the SRCGC domain
const char* SRCGCRecursiveScheduler::domain() const {
  return SRCGCdomainName;
}

// Execute the galaxy for an instant
//
// @Description Call initializeInstant() on each star, and then for
// each cluster, call run() on each star in the cluster, repeating the
// cluster as many times as necessary.  Call tick() on each star.

void SRCGCRecursiveScheduler::compileRun() {
  GalStarIter nextStar( *galaxy() );
  Star *s;

  // Run the schedule
  if ( schedule ) {
    schedule->runSchedule();
  }

  // Finish the instant by calling each star's tick() method
  nextStar.reset();

  while ( ( s = nextStar++ ) != 0 ) {
    ((SRCGCStar *) s)->tick(); // Only SR and SRCGC stars have a tick() method.
  }
}
