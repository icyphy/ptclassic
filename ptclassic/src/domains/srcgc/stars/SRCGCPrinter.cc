static const char file_id[] = "SRCGCPrinter.pl";
// .cc file generated from SRCGCPrinter.pl by ptlang
/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCPrinter.h"

const char *star_nm_SRCGCPrinter = "SRCGCPrinter";

const char* SRCGCPrinter :: className() const {return star_nm_SRCGCPrinter;}

ISA_FUNC(SRCGCPrinter,SRCGCStar);

Block* SRCGCPrinter :: makeNew() const { LOG_NEW; return new SRCGCPrinter;}

CodeBlock SRCGCPrinter :: openfile (
# line 109 "SRCGCPrinter.pl"
"    if(!($starSymbol(fp)=fopen(\"$val(fileName)\",\"w\"))) {\n"
"      fprintf(stderr,\"ERROR: cannot open output file for Printer star.\\n\");\n"
"      exit(1);\n"
"    }\n");

SRCGCPrinter::SRCGCPrinter ()
{
	setDescriptor("In each instant, print the value of the input to the filename in the\n    ``fileName'' state.  The special names &lt;stdout&gt; &lt;cout&gt;\n    &lt;stderr&gt; &lt;cerr&gt; may also be used.");
	addPort(input.setPort("input",this,ANYTYPE));
	addState(fileName.setState("fileName",this,"<stdout>","Output filename"));
	addState(prefix.setState("prefix",this,"","Prefix to print before each line of output"));


}

int SRCGCPrinter::myExecTime() {
# line 172 "SRCGCPrinter.pl"
return 6;
}

void SRCGCPrinter::wrapup() {
# line 166 "SRCGCPrinter.pl"
if(fileOutput) {
      addCode("\tfclose($starSymbol(fp));\n");
    }
}

void SRCGCPrinter::initCode() {
# line 97 "SRCGCPrinter.pl"
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

void SRCGCPrinter::setup() {
# line 89 "SRCGCPrinter.pl"
reactive();
}

void SRCGCPrinter::tick() {
# line 117 "SRCGCPrinter.pl"
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

// prototype instance for known block list
static SRCGCPrinter proto;
static RegisterBlock registerBlock(proto,"Printer");
