/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCPortHole.h"
 *                                    created: 9/03/98 15:15:23 
 *                                last update: 13/05/98 9:21:43 
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
 *  These classes are portholes for SRCCGC stars that generate C code.  
 * 
 *  History
 * 
 *  modified by  rev reason
 *  -------- --- --- -----------
 *  9/03/98  FBO 1.0 original
 * ###################################################################
 */
#ifndef _SRCGCPortHole_h
#define _SRCGCPortHole_h 1

#ifdef __GNUG__
#pragma interface
#endif

#include "CGPortHole.h"

/* Already defined in CGCPortHole  */
/* const bitWord PB_GLOBAL = 0x20; */

extern const Attribute P_GLOBAL;
extern const Attribute P_LOCAL;

class SRCGCGeodesic;

class SRCGCPortHole : public CGPortHole {
  public:
   SRCGCPortHole() : maxBuf(1), manualFlag(0), hasStaticBuf(1),
                     asLinearBuf(1), converted(0),
                     bufName(0), manualOffset(0) {}
   ~SRCGCPortHole();

   void initialize();

   void setGeodesic(Geodesic *);
   void disconnect(int);
   PortHole * far() const;
   void   connect(GenericPort &, int, const char* = 0);

   // return the far port bypassing the fork stars
   SRCGCPortHole* realFarPort();
   // const version
   const SRCGCPortHole* realFarPort() const;

   // return TRUE if the actual static buffering achieved.
   int staticBuf() const { return hasStaticBuf; }
   // give up static buffering for loop scheduling
   void giveUpStatic() { hasStaticBuf = FALSE; }


   // name the porthole in the data structure.
   void setGeoName(const char* n);
   const char* getGeoName() const;

   // Used by the SRCGCPrinter star.
   const char* getPrintFormat() const;

   // Return the geodesic connected to this PortHole.
   // This is typesafe because allocateGeodesic
   // makes myGeodesic of this type.
   SRCGCGeodesic& geo() { return *(SRCGCGeodesic*)myGeodesic;}

   // const version
   const SRCGCGeodesic& geo() const {
     return *(const SRCGCGeodesic*)myGeodesic;
   }

   // buf Size
   int bufSize() const { return 1; }

   // return bufferSize
   int inBufSize() const { return bufferSize; }

   // initialize offset
   int initOffset();

   // determine the buffer size for this porthole finally
   // Consider delay and need of past particles.
   void finalBufSize() {maxBuf = 1;}

   // Set the maxBuf manually. In finalBufSize() method, we will
   // compare this manual value with what the current scheduler
   // calculates. If this manual value is smaller, signal an error.
   void requestBufSize(int i) { maxBuf = i; manualFlag = TRUE; }

   // determine the property of the buffer associated with this port.
   void setFlags();

   // set output offset manually
   void setOffset(int v) { manualOffset = v; }
   
  private:
   int maxBuf;          // Final buffer size.
   short manualFlag;    // set if buffer size is manually chosen.
   short hasStaticBuf;  // set if static buffer is achieved.
   short asLinearBuf;   // set if acts as a linear buf inside a star
  
   int converted;       // set TRUE if the explicit type conversion 
                        // is required.

   char* bufName;       // set if no geodesic is assigned.

   int manualOffset;    // set the offset of the output manually.
};


/**********************************************************************
 *  Input Port Hole for the SRCGC domain
 **********************************************************************/
class InSRCGCPort : public SRCGCPortHole {
  public:
   int isItInput() const;

   // Return TRUE if this input has been marked as independent
   int isItIndependent() const { return isIndependent; }

   void initialize();

   // Mark this input as not instantaneously affecting any output
   //
   // @Description This should be called in a star's setup method
   // on any input that does not instanenously affect any outputs on the
   // star, e.g., the input of a delay element. <P>
   //
   // Calling this on an input that <em>does</em> instantaneously
   // affect the output may cause the system to behave
   // nondeterministically.  Not calling this on an input that does
   // not affect any output may slow the schedule, but will not affect
   // the result.
   void independent() { isIndependent = TRUE; }

  protected:
   // Flag indicating whether this input is independent
   //
   // @Description TRUE when no output instantaneously depends on this
   // input, e.g., the input of a delay element.  Set by independent().
   int isIndependent;
};


/**********************************************************************
*  Output Port Hole for the SRCGC domain
 **********************************************************************/
class OutSRCGCPort: public SRCGCPortHole {
  public:
   int isItOutput() const;     
};


class MultiInSRCGCPort : public MultiPortHole {
  public:
   // Input/output identification.
   virtual int isItInput() const;
 
   // Add a new physical port to the MultiPortHole list.
   virtual PortHole& newPort();

   PortHole & newConnection();
};

class MultiOutSRCGCPort : public MultiPortHole {     
  public:
   // Input/output identification.
   virtual int isItOutput() const;

   // Add a new physical port to the MultiPortHole list.
   virtual PortHole& newPort();
};

#endif
