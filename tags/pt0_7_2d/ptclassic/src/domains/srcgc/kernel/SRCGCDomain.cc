/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCDomain.cc"
 *                                    created: 9/03/98 15:01:52 
 *                                last update: 13/05/98 9:21:18 
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
 *  Description: 
 * 
 *  A device to produce the correct portholes, wormholes, event horizons,
 *  etc, for the SRCGC domain so the interpreter can generate them dynamically.
 *
 *  History
 * 
 *  modified by  rev reason
 *  -------- --- --- -----------
 *  9/03/98  FBO 1.0 original
 * ###################################################################
 */
static const char file_id[] = "SRCGCDomain.cc";

#include "Domain.h"
#include "SRCGCTarget.h"
#include "KnownTarget.h"
#include "SRCGCPortHole.h"
#include "SRCGCWormhole.h"
#include "SRCGCGeodesic.h"

extern const char SRCGCdomainName[] = "SRCGC";

class SRCGCDomain : public Domain {
  public:
   Star& newWorm(Galaxy& innerGal, Target* tg)  {
     LOG_NEW; return *new SRCGCWormhole(innerGal, tg);
   }

   // new fromUniversal EventHorizon
   EventHorizon& newFrom() {
     LOG_NEW; return *(new SRCGCfromUniversal);
   }

   // new toUniversal EventHorizon
   EventHorizon& newTo() {     
     LOG_NEW; return *(new SRCGCtoUniversal);
   }

   // new geodesic
   Geodesic& newGeo(int = FALSE) {
     LOG_NEW; return *new SRCGCGeodesic;
   }

   // constructor
   SRCGCDomain() : Domain("SRCGC") {subDomains += "AnyCG";}

   // require SRCGCTarget
   const char* requiredTarget() { return "SRCGCTarget";}
};

// declare a prototype
static SRCGCDomain proto;

// declare the default Target object

static SRCGCTarget defaultSRCGCtarget("default-SRCGC","SRCGCStar",
"Generate stand-alone C programs and compile them.  The program\n"
"and associated makefile is written to a directory given as a Target param.\n"
"Use the static SRRecursiveScheduler of the SR Domain.");

static KnownTarget entry(defaultSRCGCtarget,"default-SRCGC");
