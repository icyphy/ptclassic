// -*-C++-*-
// ###################################################################
//  SRCGC - Synchronous/Reactive C code generation for Ptolemy
// 
//  FILE: "SRCGCAdd.pl"
//                                    created: 17/03/98 18:19:31 
//                                last update: 13/05/98 9:24:05 
//  Author: Mathilde Roger and Vincent Legrand
//  E-mail: Frederic.Boulanger@supelec.fr
//    mail: Supélec - Service Informatique
//          Plateau de Moulon, F-91192 Gif-sur-Yvette cedex
//     www: http://wwwsi.supelec.fr/
//  
//	Thomson: Xavier Warzee <XAVIER.W.X.WARZEE@tco.thomson.fr>
//	
// Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
// All rights reserved.
// 
// Permission is hereby granted, without written agreement and without
// license or royalty fees, to use, copy, modify, and distribute this
// software and its documentation for any purpose, provided that the
// above copyright notice and the following two paragraphs appear in all
// copies of this software.
// 
// IN NO EVENT SHALL SUPELEC OR THOMSON-CSF OPTRONIQUE BE LIABLE TO ANY PARTY
// FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES
// ARISING OUT OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF
// SUPELEC OR THOMSON-CSF OPTRONIQUE HAS BEEN ADVISED OF THE POSSIBILITY OF
// SUCH DAMAGE.
// 
// SUPELEC AND THOMSON-CSF OPTRONIQUE SPECIFICALLY DISCLAIMS ANY WARRANTIES,
// INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. THE SOFTWARE
// PROVIDED HEREUNDER IS ON AN "AS IS" BASIS, AND SUPELEC OR THOMSON-CSF
// OPTRONIQUE HAS NO OBLIGATION TO PROVIDE MAINTENANCE, SUPPORT, UPDATES,
// ENHANCEMENTS, OR MODIFICATIONS.
// 
//  Description: 
// 
//  History
// 
//  modified by  rev reason
//  -------- --- --- -----------
//  17/03/98 FBO 1.0 original
// ###################################################################
//
defstar {
  name { Add }
  domain { SRCGC }
  desc { Add the two inputs. }
  version { 1.0 January 1 1998 }
  author { M. Roger & V. Legrand }
  copyright {
    Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
  }
  input {
    name { input1 }
    type { int }
  }
  input {
    name { input2 }
    type { int }
  }
  output {
    name { output }
    type { int }
  }
  setup {
    reactive();
  }
  constructor {
    noInternalState();
  }
  codeblock(code) {
    if( SRCGC_ISPRESENT($ref(input1)) && SRCGC_ISPRESENT($ref(input2)) ) {
      SRCGC_EMIT($ref(output),SRCGC_GET($ref(input1)) + SRCGC_GET($ref(input2)));
    } else if (SRCGC_ISPRESENT($ref(input1)) && SRCGC_ISABSENT($ref(input2)) ) {
      SRCGC_EMIT($ref(output),SRCGC_GET($ref(input1)));
    } else if (SRCGC_ISABSENT($ref(input1)) && SRCGC_ISPRESENT($ref(input2)) ) {
      SRCGC_EMIT($ref(output),SRCGC_GET($ref(input2)));
    }
  }
  go {
    addCode(code);
  }
  exectime {
    return 0;
  }
}
