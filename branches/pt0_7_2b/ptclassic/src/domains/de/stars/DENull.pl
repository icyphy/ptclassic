ident {
/**************************************************************************
Version identification:
$Id$

 Copyright (c) 1990 The Regents of the University of California.
                       All Rights Reserved.

 Programmer:  Soonhoi Ha
 Date of creation: 11/8/90

 It does nothing. Just connect it to any unused ports.

**************************************************************************/
}
defstar {
	name { Null }
	domain { DE }
	desc {	"It does nothing. Just connect it to any unused ports.\n"
	}
	outmulti {
		name { output }
		type { int }
	}
	go {
		// do nothing.
	}
}

