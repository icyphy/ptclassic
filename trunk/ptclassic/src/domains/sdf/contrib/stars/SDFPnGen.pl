defstar {
	name { PnGen }
	domain { SDF } 
	desc { PN Generator}
	version { $Id$ }
	author { N. Becker }
        copyright {
Copyright (c) 1990-1995 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
        }
	location { SDF user contribution library }
	output {
	  name { output }
	  type { int }
	}
	defstate {
	  name {bipolar}
	  type {int}
	  default {"NO"}
	  desc { Bipolar instead of unipolar output. }
	}
	state {
	  name {amplitude}
	  type {float}
	  default {1}
	  desc {output amplitude}
	}
	header {
#include <ACG.h>
extern ACG* gen;
	}
	go {
	  double value;
	  if( int(bipolar) ) {
	    value = ( ( (gen->asLong() & 0x01) == 0 ) ? 
			 double(amplitude) : - double(amplitude) );
	  }
	  else {
	    value = ( ( (gen->asLong() & 0x01) == 0 ) ? 
			double(amplitude) : 0 );
	  }
	  output%0 << value;
	}
}
