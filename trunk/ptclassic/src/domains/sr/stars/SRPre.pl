defstar {
  name { Pre }
  domain { SR }
  derivedFrom { SRNonStrictStar }
  desc {
Delay the input by an instant.  Absent inputs are ignored.  Output is always
present.
}
  version { @(#)SRPre.pl	1.1 4/25/96 }
  author { S. A. Edwards }
  copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
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
    if ( !output.known() ) {
        output.emit() << int(theState);
    }
  }
  
  tick {
    if ( input.present() ) {
      theState = int(input.get());
    }
  }
}
