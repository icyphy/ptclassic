defstar {
	name { DelayedMap }
	domain { HOF }
	derivedFrom { Map }
	desc {
This is a special version of the Map star for use with recursive systems.
It avoids the possibility of an unbounded recursion by separating
the block substitution from the setup method.
The setup method of this star does nothing.
The go method flags an error, it is normally required that the substitution
have occurred prior to that point.
To accomplish the substitution, the public method "substitute()" must be
explicitly called.
This method is called, for one, by the IfElseGr star.
	}
	version { $Id$ }
	author { Edward A. Lee }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { HOF main library }
	explanation {
See the explanation for
.c Map
star, from which this is derived.
.Sr "Map"
	}
	setup {
	}
	go {
	  Error::abortRun(*this,
			  "The DelayedMap star should be connected only to a star "
			  "that knows how to deal with it, like IfThenElseGr.");
	}
	method {
	  name { substitute }
	  type { void }
	  access { public }
	  arglist { "()" }
	  code {
	    HOFMap::setup();
	  }
	}
}
