defstar {
    name { Add }
    domain { SR }
    desc {
Add the two inputs.
}
    version { @(#)SRAdd.pl	1.3 4/19/96 }
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
	noInternalState();
    }
    go {
	if ( input1.present() && input2.present() ) {
	  output.emit() << int(input1.get()) + int(input2.get());
	} else if ( input1.present() && input2.absent() ) {
	  output.emit() << int(input1.get());
        } else if ( input1.absent() && input2.present() ) {
          output.emit() << int(input2.get());
        } else if ( input1.absent() && input2.absent() ) {
	  output.makeAbsent();
        }
    }
}
