defstar {
	name { DelayedMap }
	domain { HOF }
	derivedFrom { Map }
	desc {
This is a special for the Map star for use with recursive systems.
It avoids the possibility of an unbounded recursion by separating
the block substitution from the setup method.  The setup method
of this star does nothing.  The go method flags an error.
To accomplish the substitution, the public method "substitute()"
must be explicitly called.  This method is called, for one, by
the IfThenElseGr star.
	}
	version { $Id$ }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-1994 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { HOF main library }
	explanation {
See the explanation for Map star, from which this is derived.
.Sr "Map"
	}
	setup {
	}
	go {
	  Error::abortRun(*this,
			  "This star should be connected only to a star "
			  "that knows how to deal with it, like IfThenElseGr.");
	}
	method {
	  name { substitute }
	  type { void }
	  access { public }
	  arglist { "()" }
	  code {
	    initialize();
	    HOFMap::setup();
	  }
	}
}
