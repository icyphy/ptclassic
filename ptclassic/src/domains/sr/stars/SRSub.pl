defstar {
    name { Sub }
    domain { SR }
    desc {
Subtract input2 from input1
}
    version { $Id$ }
    author { S. A. Edwards }
    copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    input {
	name { input1 }
	type { int }
    }
    input {
	name { input2 }
	type { int }
    }
    output {
	name { output }
	type { int }
    }
    setup {
	reactive();
    }
    go {
	if ( input1.present() && input2.present() ) {
	    output.emit() << int(input1.get()) - int(input2.get());
	} else {
	  Error::abortRun(*this, "One input present, the other absent");
	}
    }
}
