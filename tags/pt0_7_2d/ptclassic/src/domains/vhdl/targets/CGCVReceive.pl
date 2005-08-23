defstar {
  name { VReceive }
  domain { CGC }
  desc { Receive data synchronously from VHDL to CGC }
  derivedFrom { VSynchComm }
  version { @(#)CGCVReceive.pl	1.10 10/01/96 }
  author { Michael C. Williamson, Jose Luis Pino }
  copyright { 
    Copyright (c) 1993-1997 The Regents of the University of California.
      All rights reserved.
      See the file $PTOLEMY/copyright for copyright notice,
      limitation of liability, and disclaimer of warranty provisions.
    }

  location { VHDL Target Directory }
  explanation { }
  output {
    name {output}
    type {ANYTYPE}
  }
  setup {
    classname = "CGCVReceive";
    sndrcv = "rcv";

    if (strcmp(output.resolvedType(), "INT") == 0) 
      format = "%d";
    else if (strcmp(output.resolvedType(), "FLOAT") == 0) 
/*
  Note: using %f in sscanf() seems to mess up.
      format = "%f";
*/
      format = "%lf";
    else
      Error::abortRun(*this, output.resolvedType(), ": type not supported");

    numXfer = output.numXfer();
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
      transfer << "    $starSymbol(status) = read($starSymbol(xmitsock), $starSymbol(buffer), $starSymbol(nbytes));\n";
      transfer << "    while($starSymbol(status) == 0) {\n";
      transfer << "      $starSymbol(status) = read($starSymbol(xmitsock), $starSymbol(buffer), $starSymbol(nbytes));\n";
      transfer << "    }\n";
      transfer << "    if($starSymbol(status) < 0) {\n";
      transfer << "      perror($starSymbol(dummy));\n";
      transfer << "    }\n";
      transfer << "    else {\n";
      
      StringList oneline = "      (void) sscanf($starSymbol(buffer), \"";
      oneline << format;
      oneline << "\", &$ref(output,";
      oneline << i;
      oneline << "));";
      transfer << oneline;

      transfer << "    }\n";
      transfer << "  }\n";
    }
    addCode(transfer);
  }

}
