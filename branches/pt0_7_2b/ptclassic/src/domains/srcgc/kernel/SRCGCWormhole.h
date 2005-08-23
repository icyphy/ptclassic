/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCWormhole.h"
 *                                    created: 9/03/98 15:34:12 
 *                                last update: 13/05/98 9:22:01 
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
 *  History
 * 
 *  modified by  rev reason
 *  -------- --- --- -----------
 *  9/03/98  FBO 1.0 original
 * ###################################################################
 */
#ifndef _SRCGCWormhole_h
#define _SRCGCWormhole_h 1
#ifdef __GNUG__
#pragma interface
#endif


#include "StringList.h"
#include "EventHorizon.h"
#include "SRCGCPortHole.h"
#include "CGWormBase.h"
#include "SRCGCStar.h"


        //////////////////////////////////////////
        // class SRCGCtoUniversal
        //////////////////////////////////////////

class SRCGCWormhole : public CGWormBase, public SRCGCStar {
  public:
   // Constructor
   SRCGCWormhole(Galaxy& g, Target* t = 0);
   ~SRCGCWormhole();

   // return my scheduler
   Scheduler* scheduler() const { return myTarget()->scheduler() ;}

   // execution time which is the average of the workload inside
   // the wormhole with 1 processor.
   int myExecTime() { return execTime; }

   // print methods
   StringList print(int verbose = 0) const {
     return Wormhole::print(verbose);
   }
  
   // clone -- allows interpreter/pigi to make copies
   Block* clone() const;

   // identify myself as a wormhole
   int isItWormhole() const { return TRUE;}

   // use statelist for inner galaxy for stateWithName
   State* stateWithName (const char* name) {
     return gal.stateWithName(name);
   }

   // state initialize
   void initState() { gal.initState() ;}

   // FIXME: what should this do?
   double getStopTime() { return 0.0;}

   // return myself
   /*virtual*/ Wormhole* asWormhole();

  protected:
   /*virtual*/ void go();
};

// Input Boundary of ??inSRCGC_Wormhole
class SRCGCtoUniversal : public ToEventHorizon, public InSRCGCPort {
  public:
   // constructor
   SRCGCtoUniversal() : ToEventHorizon(this) {}

   // redefine
   void receiveData() ;

   void initialize();

   int isItInput() const;
   int isItOutput() const;

   // as EventHorizon
   EventHorizon* asEH();

   /*virtual*/ Geodesic* allocateGeodesic() {
     return ToEventHorizon::allocateGeodesic();
   }

   int onlyOne() const;
};


        //////////////////////////////////////////
        // class SRCGCfromUniversal
        //////////////////////////////////////////

// Output Boundary of ??inSRCGC_Wormhole
class SRCGCfromUniversal : public FromEventHorizon, public OutSRCGCPort {
  protected:
   // redefine the "ready" method for implicit synchronization.
   int ready() ;

  public:
   // constructor
   SRCGCfromUniversal() : FromEventHorizon(this) {}

   // redefine
   void sendData(); 

   void initialize();

   int isItInput() const;
   int isItOutput() const;

   // as EventHorizon
   EventHorizon* asEH();

   /*virtual*/ Geodesic* allocateGeodesic() {
     return FromEventHorizon::allocateGeodesic();
   }

   int onlyOne() const;
};
  
#endif
