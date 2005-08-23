// -*-C++-*-
// ###################################################################
//  SRCGC - Synchronous/Reactive C code generation for Ptolemy
// 
//  FILE: "SRCGCPre.pl"
//                                    created: 17/03/98 18:01:16 
//                                last update: 13/05/98 9:24:25 
//  Author: Mathilde Roger and Vincent Legrand
//  E-mail: Frederic.Boulanger@supelec.fr
//    mail: Sup�lec - Service Informatique
//          Plateau de Moulon, F-91192 Gif-sur-Yvette cedex
//     www: http://wwwsi.supelec.fr/
//  
//	Thomson: Xavier Warzee <XAVIER.W.X.WARZEE@tco.thomson.fr>
//	
// Copyright (c) 1998 Sup�lec & Thomson-CSF Optronique.
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
  name { Pre }
  domain { SRCGC }
  derivedFrom { SRCGCNonStrictStar }
  desc {
    Delay the input by an instant.  Absent inputs are ignored.
    Output is always present.
  }
  version { 1.0 January 1 1998 }
  author { V. Legrand & M. Roger}
  copyright {
    Copyright (c) 1998 Sup�lec & Thomson-CSF Optronique.
    All rights reserved.
  }

  input {
    name { input }
    type { int }
  }

  output {
    name { output }
    type { int }
  }

  state {
    name { theState }
    type { int }
    default { "0" }
    desc { Initial output value, state afterwards. }
  }

  ccinclude {<stream.h>}
  
  setup {
    input.independent();
  }

  go {  
    addCode("\tif (!SRCGC_ISKNOWN($ref(output))) {\n"
            "\tSRCGC_EMIT($ref(output),$ref(theState));\n\t}\n");
  }
  
  tick {
    addCode("\tif ( SRCGC_ISPRESENT($ref(input))) {\n"
            "\t $ref(theState) = SRCGC_GET($ref(input));\n\t}\n");
  }
}
