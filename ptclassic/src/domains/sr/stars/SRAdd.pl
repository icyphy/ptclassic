defstar {
    name { Add }
    domain { SR }
    desc {
Add the two inputs.
}
    version { $Id$ }
    author { S. A. Edwards }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
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
	type { output }
    }
    go {
	output.emit() << int(input1.get()) + int(input2.get());
    }
}
