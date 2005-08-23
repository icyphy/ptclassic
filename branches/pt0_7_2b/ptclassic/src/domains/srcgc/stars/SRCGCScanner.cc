static const char file_id[] = "SRCGCScanner.pl";
// .cc file generated from SRCGCScanner.pl by ptlang
/*
Copyright (c) 1998 Supélec & Thomson-CSF Optronique.
    All rights reserved.
 */

#ifdef __GNUG__
#pragma implementation
#endif

#include "SRCGCScanner.h"

const char *star_nm_SRCGCScanner = "SRCGCScanner";

const char* SRCGCScanner :: className() const {return star_nm_SRCGCScanner;}

ISA_FUNC(SRCGCScanner,SRCGCStar);

Block* SRCGCScanner :: makeNew() const { LOG_NEW; return new SRCGCScanner;}

CodeBlock SRCGCScanner :: openfile (
# line 97 "SRCGCScanner.pl"
"    if(!($starSymbol(fp)=fopen(\"$val(fileName)\",\"r\"))) {\n"
"      fprintf(stderr,\"ERROR: cannot open input file for Scanner star.\\n\");\n"
"      exit(1);\n"
"    }\n");

SRCGCScanner::SRCGCScanner ()
{
	setDescriptor("In each instant, print the value of the input to the filename in the\n    ``fileName'' state.  The special names &lt;stdout&gt; &lt;cout&gt;\n    &lt;stderr&gt; &lt;cerr&gt; may also be used.");
	addPort(output.setPort("output",this,INT));
	addState(fileName.setState("fileName",this,"<stdin>","Output filename"));


}

int SRCGCScanner::myExecTime() {
# line 124 "SRCGCScanner.pl"
return 0;
}

void SRCGCScanner::wrapup() {
# line 118 "SRCGCScanner.pl"
if(fileInput) {
      addCode("\tfclose($starSymbol(fp));\n");
    }
}

void SRCGCScanner::initCode() {
# line 84 "SRCGCScanner.pl"
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

void SRCGCScanner::go() {
# line 105 "SRCGCScanner.pl"
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

// prototype instance for known block list
static SRCGCScanner proto;
static RegisterBlock registerBlock(proto,"Scanner");
