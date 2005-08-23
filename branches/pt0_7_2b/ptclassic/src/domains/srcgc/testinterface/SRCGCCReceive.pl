defstar {
  name { CReceive }
  domain { SRCGC }
  desc { CGC to SRCGC Receive star }
  version { @(#)SRCGCCReceive.pl  1.16  01 Oct 1996 }
  author { :-) }
  derivedFrom {CSynchComm}
  copyright {
    Copyright (c) 1993-1996 The Regents of the University of California.
    All rights reserved.
    See the file $PTOLEMY/copyright for copyright notice,
    limitation of liability, and disclaimer of warranty provisions.
  }
  location { SRCGC Target Directory }
  ccinclude { "CGCSRReceive.h" }
  htmldoc {
  }

  output {
    name {output}
    type {ANYTYPE}
  }

  setup {
    SRCGCCSynchComm::setup();      
  }

  go {
    addCode("machin SRCGCCReceive");
  }

  execTime {
    0;
  }
}
