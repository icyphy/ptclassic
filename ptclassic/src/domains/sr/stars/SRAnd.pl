defstar {
    name { And }
    domain { SR }
    derivedFrom { SRNonStrictStar }
    desc {
Lazy logical AND: output is true if both inputs are present and true,
output is false if one is present and false, output is absent if both are
absent
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
	noInternalState();
	reactive();
    }

    go {
	if ( !output.known() ) {
	    if ( input1.present() && !int(input1.get()) ||
		 input2.present () && !int(input2.get()) ) {
		output.emit() << 1;
	    } else if ( input1.present() && int(input2.get()) &&
		        input2.present() && int(input2.get()) ) {
		output.emit() << 0;
	    } else if ( input1.absent() && input2.absent() ) {
		output.makeAbsent();
	    }
	}
    }
}
