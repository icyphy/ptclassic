defstar {
    name { When }
    domain { SR }
    derivedFrom { SRNonStrictStar }
    desc {
When the clock input is true, copy the input to the output;
when the clock is absent, make the output absent.
}
    version { $Id$ }
    author { S. A. Edwards }
    copyright {
Copyright (c) 1996-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    input {
	name { input }
	type { int }
    }
    input {
	name { clock }
	type { int }
    }
    output {
	name { output }
	type { int }
    }

    setup {
	noInternalState();
	reactive();
    }

    go {
	if ( !output.known() && clock.known() ) {
	    if ( clock.present() ) {
	 	if ( int(clock.get()) ) {

		// clock is present and true--copy input to output

		    if ( input.known() ) {
			if ( input.present() ) {

			    // input is present--copy it

			    output.emit() << int(input.get());

			} else {

			    // input is absent

			    output.makeAbsent();
			}
		    }

		} else {

		// clock is present and false

	        output.makeAbsent();

		}
		
	    } else {

		// clock is absent--make the output absent

		output.makeAbsent();
	    }
	}
    }
}

