defstar {
	name   { BlackHole }
	domain { CGC }
	desc   {
Swallows an input sample
	}
	version     { $Id$ }
	author      { E. Ammicht }
WARNING: CHECK COPYRIGHT - not UC
	copyright   { 1992 AT&T Bell Laboratories }
	location    { CGC main library }

	explanation {
This star absorbs all inputs, hence no code needs to be generated!
	}

	inmulti {
		name { input }
		type { ANYTYPE }
	}

	constructor {
		noInternalState();
	}
	go {
		addCode("/* BLACK HOLE */" );
	}
}

