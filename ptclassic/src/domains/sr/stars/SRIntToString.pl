defstar {
  name { IntToString }
  domain { SR }
  derivedFrom { SRStar }
  desc { Convert an integer input to a string output }
  version { $Id$ }
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

  setup {
    noInternalState();
    reactive();
  }

  go {
    if ( !output.known() ) {
      if ( input.absent() ) {
	output.makeabsent();
      } else {
	char theString[10];
	sprintf( theString, "%d", input.get() );
	output.emit() << theString;
      }
    }
  }
}
