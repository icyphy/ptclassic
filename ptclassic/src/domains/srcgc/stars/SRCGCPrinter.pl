// -*-C++-*-
// ###################################################################
//  SRCGC - Synchronous/Reactive C code generation for Ptolemy
// 
//  FILE: "SRCGCPrinter.pl"
//                                    created: 17/03/98 18:02:04 
//                                last update: 13/05/98 9:24:27 
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
  name { Printer }
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
    This star prints its input, which may be any supported data type.
    <p>
    If output is directed to a file, then flushing does not occur until
    the wrapup method is called.
    Before the first data are flushed, the file will not exist.
  }
  inmulti {
    name { input }
    type { ANYTYPE }
  }

  defstate {
    name { fileName }
    type { string }
    default { "<stdout>" }
    desc { Output filename }
  }

  defstate {
      name { prefix }
      type { string }
      default { "" }
      desc { Prefix to print before each line of output }
  }

  setup {
    reactive();
  }
  
  private {
    int fileOutput;
  }
  
  initCode{
    const char *fn = fileName;
    fileOutput = ! (  fn==NULL
                    ||strcmp(fn, "cout")==0 || strcmp(fn, "stdout")==0
                    ||strcmp(fn, "<cout>")==0 || strcmp(fn, "<stdout>")==0);
    if(fileOutput) {
      StringList s;
      s << "    FILE* $starSymbol(fp);";
      addDeclaration(s);
      addInclude("<stdio.h>");
      addCode(openfile);
    }
  }
  codeblock (openfile) {
    if(!($starSymbol(fp)=fopen("$val(fileName)","w"))) {
      fprintf(stderr,"ERROR: cannot open output file for Printer star.\n");
      exit(1);
    }
  }

  tick {
    // Print the prefix
    StringList code ;
    if(fileOutput) {    
      code = "\tfprintf($starSymbol(fp),\"%s\",$ref(prefix));\n";
    } else {
      code = "\tprintf(\"%s\",$ref(prefix));\n";
    }
    // Print the value of each output

    MPHIter nexti(input);
    InSRCGCPort * p;   
    while ( (p = (InSRCGCPort *)(nexti++)) != NULL ) {
      code << "\tif ( !SRCGC_ISKNOWN(" << p -> getGeoName() << ") ) {\n";
      if(fileOutput) {
        code << "\t\tfprintf($starSymbol(fp),\"unknown \");\n";
      } else {
        code << "\t\tprintf(\"unknown \");\n";
      }
      code << "\t} else {\n";
      code << "\t\tif (SRCGC_ISPRESENT(" << p -> getGeoName() << ") ) {\n";
     
      if(fileOutput) {
        code << "\t\t\tfprintf($starSymbol(fp),\"" << p -> getPrintFormat();
        code << " \",SRCGC_GET(" << p -> getGeoName() << ") );\n";
      } else {
        code << "\t\t\tprintf(\"" << p -> getPrintFormat();
        code << " \",SRCGC_GET(" << p -> getGeoName() << ") );\n";
      }
      code << "\t\t} else {\n";
      
      if(fileOutput) {
        code << "\t\t\tfprintf($starSymbol(fp),\"absent \");\n";
      } else {
        code << "\t\t\tprintf(\"absent \");\n";
      }

      code << "\t\t}\n\t}\n";
    }
    
    if(fileOutput) {
      code << "\tfprintf($starSymbol(fp),\"\\n\");\n";
    } else {
      code << "\tprintf(\"\\n\");\n";
    }
    
    addCode(code);
  }

  wrapup {
    if(fileOutput) {
      addCode("\tfclose($starSymbol(fp));\n");
    }
  }
  
  exectime {
    return 6;
  }
}
