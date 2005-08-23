defstar {
  name { SRReceive }
  domain { CGC }
  desc { Receive data synchronously from SRCGC to CGC }
  derivedFrom { SRSynchComm }
  version { @(#)CGCSRReceive.pl 1.21 01 Oct 1996 }
  author { Jose Luis Pino }
  copyright { 
    Copyright (c) 1993-1996 The Regents of the University of California.
    All rights reserved.
    See the file $PTOLEMY/copyright for copyright notice,
    limitation of liability, and disclaimer of warranty provisions.
  }
  
  location { SRCGC Target Directory }
  htmldoc {
  }
  output {
    name {output}
    type {ANYTYPE}
  }

  setup {
   CGCSRSynchComm::setup();
 }

 go {
   addCode("coucou CGCSRReceive");
 }
}
