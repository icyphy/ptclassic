defstar {
	name   { BlackHole }
	domain { CGC }
	desc   {
Swallows an input sample
	}
	version     { $Id$ }
	author      { E. Ammicht }
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
		addCode( CodeBlock( "/* BLACK HOLE */" ));
	}
}

