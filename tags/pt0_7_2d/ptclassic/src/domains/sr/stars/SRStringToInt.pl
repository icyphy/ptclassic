defstar {
  name { StringToInt }
  domain { SR }
  derivedFrom { SRStar }
  desc { Convert a string input to an integer output }
  version { @(#)SRStringToInt.pl	1.1 01/27/97 }
  author { S. A. Edwards }
  copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
  }

  input {
    name { input }
    type { string }
  }

  output {
    name { output }
    type { int }
  }

  setup {
    noInternalState();
    reactive();
  }

  go {
    if ( !output.known() ) {
      if ( input.absent() ) {
	output.makeAbsent();
      } else {
	int val = atoi( input.get().print() );
	output.emit() << val;       
      }
    }
  }
}
