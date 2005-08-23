defstar {
  name { SRSend }
  domain { CGC }
  desc { Send data synchronously from CGC to S56X }
  derivedFrom { SRSynchComm }
  version { @(#)CGCSRSend.pl  1.18  01 Oct 1996 }
  author {  }
  
  copyright { 
    Copyright (c) 1993-1996 The Regents of the University of California.
    All rights reserved.
    See the file $PTOLEMY/copyright for copyright notice,
    limitation of liability, and disclaimer of warranty provisions.
  }

  location { SRCGC Target Directory }
  
  explanation {}
  
  input {
    name {input}
    type {ANYTYPE}
  }
  
  setup {
   CGCSRSynchComm::setup();
 }
 
 go {
   addCode("coucou CGCSRSend.pl\n");
 }
}
