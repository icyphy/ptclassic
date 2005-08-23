defstar {
  name { CSend }
  domain { SRCGC }
  desc { SRCGC to CGC sunchronous send star }
  version { @(#)SRCGCCSend.pl 1.16  01 Oct 1996 }
  author {  }
  derivedFrom { CSynchComm }

  copyright {
    Copyright (c) 1993-1996 The Regents of the University of California.
    All rights reserved.
    See the file $PTOLEMY/copyright for copyright notice,
    limitation of liability, and disclaimer of warranty provisions.
  }

  ccinclude { "CGTarget.h" }
  
  location { SRCGC Target Directory }
  explanation {}

  input {
    name {input}
    type {ANYTYPE}
  }

  setup {
    SRCGCCSynchComm::setup();
  }
  
  go {
    addCode("couco SRCGCCSend.pl");
  }

  execTime {
    0;
  }
}
