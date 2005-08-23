/* -*-C++-*-
 * ###################################################################
 *  SRCGC - Synchronous/Reactive C code generation for Ptolemy
 * 
 *  FILE: "SRCGCNonStrictStar.cc"
 *                                    created: 9/03/98 15:07:45 
 *                                last update: 13/05/98 9:21:35 
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
static const char file_id[] = "SRCGCNonStrictStar.cc";

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCNonStrictStar.h"
#include "Block.h"

extern const char SRCGCdomainName[];

// Class identification.
ISA_FUNC(SRCGCNonStrictStar, SRCGCStar);

// Domain identification.
const char * SRCGCNonStrictStar::domain() const {
  return SRCGCdomainName;
}

// Compute the output of the star
//
// @Description For this non-strict star, it always calls go().
int SRCGCNonStrictStar::run() {
  StringList code = "\t{  /* star ";
  code << fullName() << " (class " << className() << ") */\n";
  addCode(code);
  int status = Star::run();
  addCode("\t}\n");
  
  return status;  
}
