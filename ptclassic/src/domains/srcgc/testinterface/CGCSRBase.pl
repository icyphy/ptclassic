defstar {
  name { SRBase }
  domain { CGC }
  derivedFrom { Fix }
  desc { Base star from CGC <-> SRCGC  }
  version { @(#)CGCSRBase.pl  1.26  10/01/96 }
  author { :-) }
  copyright { 
    Copyright (c) 1993-1997 The Regents of the University of California.
    All rights reserved.
    See the file $PTOLEMY/copyright for copyright notice,
    limitation of liability, and disclaimer of warranty provisions.
  }
  location { SRCGC Target Directory }
  htmldoc {
  }
  
//ccinclude { <stdio.h>,"compat.h" }

  public {
    friend class SRCGCCommTarget;
  }

  initCode {
   CGCFix::initCode();
 }

 wrapup {
   CGCFix::wrapup();
 }
}
