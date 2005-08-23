defstar {
  name { VSend }
  domain { CGC }
  desc { Send data synchronously from CGC to VHDL }
  derivedFrom { VSynchComm }
  version { @(#)CGCVSend.pl	1.12 10/01/96 }
  author { Michael C. Williamson, Jose Luis Pino }
  copyright { 
    Copyright (c) 1993-1997 The Regents of the University of California.
      All rights reserved.
      See the file $PTOLEMY/copyright for copyright notice,
      limitation of liability, and disclaimer of warranty provisions.
    }

  location { VHDL Target Directory }
  explanation { }
  input {
    name {input}
    type {ANYTYPE}
  }
  setup {
    classname = "CGCVSend";
    sndrcv = "snd";

    if (strcmp(input.resolvedType(), "INT") == 0) 
      format = "%d";
    else if (strcmp(input.resolvedType(), "FLOAT") == 0) 
/*
  Note: using %f in sscanf() seems to mess up.
      format = "%f";
*/
// This works
//      format = "%lf";

// Try this %g:
      format = "%g";
    else
      Error::abortRun(*this, input.resolvedType(), ": type not supported");

    numXfer = input.numXfer();
    CGCVSynchComm::setup();
  }

  go {
    StringList transfer;
    for (int i = numXfer-1 ; i >= 0 ; i--) {
      if (i > 0) transfer << "\n";
      transfer << "\n";
      transfer << "  /* Go */\n";
      transfer << "  $starSymbol(intptr) = 0;\n";
      transfer << "  $starSymbol(status) = 0;\n";
      transfer << "  if($starSymbol(status) >= 0) {\n";

      StringList oneline = "    (void) sprintf($starSymbol(buffer), \"";
      oneline << format;
      oneline << "\", $ref(input,";
      oneline << i;
      oneline << "));";
      transfer << oneline;

      transfer << "\n";
      transfer << "    $starSymbol(status) = write($starSymbol(xmitsock), $starSymbol(buffer), $starSymbol(nbytes));\n";
      transfer << "    if($starSymbol(status) < 0) {\n";
      transfer << "      perror($starSymbol(dummy));\n";
      transfer << "    }\n";
      transfer << "  }\n";
    }
    addCode(transfer);
  }

}
