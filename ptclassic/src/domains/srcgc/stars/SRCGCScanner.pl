// -*-C++-*-
// ###################################################################
//  SRCGC - Synchronous/Reactive C code generation for Ptolemy
// 
//  FILE: "SRCGCScanner.pl"
//                                    created: 17/03/98 18:02:28 
//                                last update: 13/05/98 9:24:28 
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
  name { Scanner }
  domain { SRCGC }

  desc {
    In each instant, print the value of the input to the filename in the
    ``fileName'' state.  The special names &lt;stdout&gt; &lt;cout&gt;
    &lt;stderr&gt; &lt;cerr&gt; may also be used.
  }

  version { 1.0 January 1 1998 }
  author { V. Legrand & M. Roger}
  copyright {
    Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
  }

  htmldoc {
    This star reads values from a file or standard input.
  }
  output {
    name { output }
    type { int }
  }

  defstate {
    name { fileName }
    type { string }
    default { "<stdin>" }
    desc { Output filename }
  }

  private {
    int fileInput;
  }
  
  initCode{
    const char *fn = fileName;
    fileInput = ! (  fn==NULL
                   ||strcmp(fn, "cin")==0 || strcmp(fn, "stdin")==0
                   ||strcmp(fn, "<cin>")==0 || strcmp(fn, "<stdin>")==0);
    StringList s;
    if(fileInput) {
      s << "\tFILE* $starSymbol(fp);\n";
      addInclude("<stdio.h>");
      addCode(openfile);
    }
    s << "\tint $starSymbol(val);\n";
    addDeclaration(s);
  }
  codeblock (openfile) {
    if(!($starSymbol(fp)=fopen("$val(fileName)","r"))) {
      fprintf(stderr,"ERROR: cannot open input file for Scanner star.\n");
      exit(1);
    }
  }

  go {
    StringList code;
    // Read the value 
    if (fileInput) {
      code << "\tfscanf($starSymbol(fp),\"%u\",&$starSymbol(val));\n";
    } else {
      code << "\tscanf(\"%u\",&$starSymbol(val));\n";
    }
    code << "\tSRCGC_EMIT($ref(output),$starSymbol(val));\n";
    
    addCode(code);
  }
  
  wrapup {
    if(fileInput) {
      addCode("\tfclose($starSymbol(fp));\n");
    }
  }
  
  exectime {
    return 0;
  }
}
