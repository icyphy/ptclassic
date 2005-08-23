defstar {
  name { CSynchComm }
  domain { SRCGC }
  desc { SRCGC to CGC send/receive base star }
  version { @(#)SRCGCCSynchComm.pl  1.7 01 Oct 1996 }
  author {  }
  copyright {
    Copyright (c) 1993-1996 The Regents of the University of California.
    All rights reserved.
    See the file $PTOLEMY/copyright for copyright notice,
    limitation of liability, and disclaimer of warranty provisions.
  }
  location { SRCGC Target Directory }
  explanation { }
  
  header {
    class CGCSRSynchComm;
  }

  public {
    CGCSRSynchComm* cgcSide;
  }

  setup {
  }

  initCode {
  }
}
