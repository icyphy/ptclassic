ident {
// Copyright (c) 1990 The Regents of the University of California.
//			All Rights Reserved.

// This star generates (int-1), (int-2), ... 1,0 samples for (int) input.
// Written by: Soonhoi Ha
// Date of creation: 8/28/90
// Converted to use preprocessor, 12/1/90
// 
// $Id$
}

defstar {
	name { DownCounter }
	domain { DDF }
	desc { "A DownCounter from input value to zero" }
	input {
		name { input }
		type { int }
	}
	output {
		name { output }
		type { int }
		num { 0 }
	}
	go {
		// get input token from Geodesic
		input.grabData();

		// generates output
		int in = int (input%0);
		for (int i = in - 1 ; i >= 0; i--) {
			output%0 << i ;
			output.sendData();
		}
	}
}

