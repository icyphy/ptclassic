defstar {
  name { SRSynchComm }
  domain { CGC }
  //derivedFrom { SRBase }
  desc { Base star for Synchronous CGC <-> SRCGC  }
  version { @(#)CGCSRSynchComm.pl 1.12 01 Oct 1996 }
  author { Jose Luis Pino }
  hinclude { "SRCGCCSynchComm.h" }
  copyright { 
    Copyright (c) 1993-1996 The Regents of the University of California.
    All rights reserved.
    See the file $PTOLEMY/copyright for copyright notice,
    limitation of liability, and disclaimer of warranty provisions.
  }
  location { SRCGC Target Directory }
  htmldoc {
  }
  
  ccinclude {  }
  
  public {
    SRCGCCSynchComm* srcgcSide;
  }
  
  initCode {
  }
}
