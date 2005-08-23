/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCRecursiveScheduler.h"
 *                                    created: 9/03/98 16:41:58 
 *                                last update: 13/05/98 9:21:48 
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
 *  SRCGC uses the static recursive scheduler of SR.
 *  All we have to do here is to change the domain and the compileRun
 *  method to use SRCGC stars instead of SR stars
 * 
 *  History
 * 
 *  modified by  rev reason
 *  -------- --- --- -----------
 *  9/03/98  FBO 1.0 original
 * ###################################################################
 */

#ifndef _SRCGCRecursiveScheduler_h
#define _SRCGCRecursiveScheduler_h

#ifdef __GNUG__
#pragma interface
#endif

#include "SRRecursiveScheduler.h"

class SRCGCRecursiveScheduler : public SRRecursiveScheduler {
  public:
   const char* domain() const;
   /*virtual*/ void compileRun();
};

#endif
