defstar {
  name { VReceive }
  domain { CGC }
  desc { Receive data synchronously from VHDL to CGC }
  derivedFrom { VSynchComm }
  version { $Id$ }
  author { Michael C. Williamson, Jose Luis Pino }
  copyright { 
    Copyright (c) 1994,1993 The Regents of the University of California.
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
      format = "%f";
    else
      Error::abortRun(*this, output.resolvedType(), ": type not supported");

    numXfer = output.numXfer();
    CGCVSynchComm::setup();
  }

  go {
    addCode("
  /* Go */
  $starSymbol(intptr) = 0;
  $starSymbol(status) = 0;
  if($starSymbol(status) >= 0) {
    $starSymbol(status) = read($starSymbol(xmitsock), $starSymbol(buffer), $starSymbol(nbytes));
    while($starSymbol(status) == 0) {
      $starSymbol(status) = read($starSymbol(xmitsock), $starSymbol(buffer), $starSymbol(nbytes));
    }
    if($starSymbol(status) < 0) {
      perror($starSymbol(dummy));
    }
    else {
");

    StringList oneline = "      (void) sscanf($starSymbol(buffer), \"";
    oneline << format;
    oneline << "\", &$ref(output));";
    addCode(oneline);
		    
    addCode("    }
  }
");
  }

}
