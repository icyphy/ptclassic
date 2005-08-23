defstar
{
    name { FileSource }
    domain { SDF }
    version { @(#)SDFFileSource.pl	1.3	2/21/96 }
    author { E. A. Lee }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liabi
lity, and disclaimer of warranty provisions.
    }
    location { SDF dfm library }
    descriptor {
Output the given file name on each firing.
If no name is given, then output a unique name of a temporary file
on each firing.
    }
    ccinclude { "FileMessage.h" <iostream.h> }

    output {
	name { output }
	type { filemsg }
    }

    defstate {
	name { fileName }
	type { string }
	default { "" }
	descriptor { File name to produce (use temporary file if empty). }
    }
    go {
	FileMessage* fm;
	const char* fn = (const char*) fileName;
	if (*fn == '\0') {
	    fm = new FileMessage();
	} else {
	    fm = new FileMessage(fn);
	}
	Envelope pkt(*fm);
	output%0 << pkt;
    }
}
