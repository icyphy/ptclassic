/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCGeodesic.h"
 *                                    created: 9/03/98 15:06:31 
 *                                last update: 13/05/98 9:19:12 
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
 *  Geodesic for C code generation.
 *  The geodesic for the SRCGC domain
 *
 *  @Description This implements the single-driver, multiple-receiver
 *  communication channels of the SRCGC domain.
 *
 *  <P> originatingPort defines the driver, but receivers defines the list
 *  of receivers.  The Geodesic::destinationPort is ignored.
 *  
 *  History
 * 
 *  modified by  rev reason
 *  -------- --- --- -----------
 *  9/03/98  FBO 1.0 original
 * ###################################################################
 */
#ifndef _SRCGCGeodesic_h
#define _SRCGCGeodesic_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CGGeodesic.h"


class SRCGCGeodesic : public CGGeodesic {
  public:
   SRCGCGeodesic() : bufName(0), myWaste(2.0) {}
   ~SRCGCGeodesic();

   PortHole * setSourcePort( GenericPort &, int, const char * = 0 );
   PortHole * setOldSourcePort( GenericPort &, int, const char * = 0 );
   PortHole * setDestPort( GenericPort & );
   PortHole * setOldDestPort( GenericPort & );

   PortHole * getReceiver();

   int isItPersistent() const;
   int disconnect( PortHole & );  

   // class identification
   int isA(const char*) const;

   void setBufName(const char* n) {
     delete [] bufName; bufName = savestring(n);
   }
   char* getBufName() const;

   // make large the wasteFactor
   void preferLinearBuf(int i) { if (i) myWaste = 1e9; }

  protected:
   // Redefine: decide the wasteFactor for linear buffering.
   // Look at the comment in CGGeodesic class.
   double wasteFactor() const;

  private:
   // A list of InSRCGCPort *s driven by this geodesic 
   // @Description This supplants the destinationPort field of the
   // Geodesic class
   SequentialList receivers;

   // Name of the C object used as a buffer
   char* bufName;
   // wasteFactor
   double myWaste;
};

#endif
