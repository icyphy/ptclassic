defstar
{
    name { ConstString }
    domain { SDF }
    version { @(#)SDFConstString.pl	1.1	2/29/96 }
    author { E. A. Lee and John Reekie }
    copyright {
Copyright (c) 1990-1996 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liabi
lity, and disclaimer of warranty provisions.
    }
    location { SDF dfm library }
    descriptor {
Output the given string on each firing.
    }
    ccinclude { "StringMessage.h" <iostream.h> }

    output {
	name { output }
	type { string }
    }

    defstate {
	name { stringValue }
	type { string }
	default { "" }
	descriptor { String value to produce }
    }
    go {
	StringMessage* fm;
	const char* fn = (const char*) stringValue;
	fm = new StringMessage(fn);
	Envelope pkt(*fm);
	output%0 << pkt;
    }
}
