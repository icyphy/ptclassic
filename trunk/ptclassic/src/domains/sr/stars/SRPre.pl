defstar {
  name { Pre }
  domain { SR }
  derivedFrom { SRNonStrictStar }
  desc {
Delay a present input until the instant in which the next present instant
occurs.
}
  version { @(#)SRPre.pl	1.1 4/25/96 }
  author { S. A. Edwards }
  copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }

  input {
    name { input }
    type { int }
  }

  output {
    name { output }
    type { int }
  }

  state {
    name { theState }
    type { int }
    default { "0" }
    desc { Initial output value, state afterwards. }
  }

  ccinclude {<stream.h>}
  
  setup {
    input.independent();
  }

  go {	
    if ( !output.known() && input.present() ) {
      output.emit() << int(theState);
    }
  }
  
  tick {
    if ( input.present() ) {
      theState = int(input.get());
    }
  }
}
