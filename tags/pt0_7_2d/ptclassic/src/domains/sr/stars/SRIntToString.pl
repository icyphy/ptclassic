defstar {
  name { IntToString }
  domain { SR }
  derivedFrom { SRStar }
  desc { Convert an integer input to a string output }
  version { @(#)SRIntToString.pl	1.4 09/10/99 }
  author { S. A. Edwards }
  copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
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
    type { string }
  }

  // sprintf()
  ccinclude{ <stdio.h> }

  setup {
    noInternalState();
    reactive();
  }

  go {
    if ( !output.known() ) {
      if ( input.absent() ) {
	output.makeAbsent();
      } else {
	char theString[10];
	sprintf( theString, "%d", int(input.get()) );
	output.emit() << theString;
      }
    }
  }
}
