defstar {
  name { VSend }
  domain { CGC }
  desc { Send data synchronously from CGC to VHDL }
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
      format = "%f";
    else
      Error::abortRun(*this, input.resolvedType(), ": type not supported");

    numXfer = input.numXfer();
    CGCVSynchComm::setup();
  }

  go {
    addCode("
  /* Go */
  $starSymbol(intptr) = 0;
  $starSymbol(status) = 0;
  if($starSymbol(status) >= 0) {
");

    StringList oneline = "  (void) sprintf($starSymbol(buffer), \"";
    oneline << format;
    oneline << "\", $ref(input));";
    addCode(oneline);
    
    addCode("$starSymbol(status) = write($starSymbol(xmitsock), $starSymbol(buffer), $starSymbol(nbytes));
    if($starSymbol(status) < 0) {
      perror($starSymbol(dummy));
    }
  }
");
  }

}
