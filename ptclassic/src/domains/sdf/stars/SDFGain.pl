ident {
// A variable gain star.
// J. Buck
// $Id$
// Copyright (c) 1990 The Regents of the University of California.
//			All Rights Reserved.
}
defstar {
	name { FloatGain }
	domain { SDF }
	desc { "Amplifier: output is input times gain (default 1.0)" }
	input {
		name { input }
		type { float }
	}
	output {
		name { output }
		type { float }
	}
	defstate {
		name { gain }
		type { float }
		default { "1.0" }
		desc { "gain of the star" }
	}
	go {
		output%0 << double(gain) * (float)(input%0);
	}
}

