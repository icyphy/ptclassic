defstar {
    name { Mux }
    domain { SR }
    derivedFrom { SRNonStrictStar }
    desc {
When the select input is true, copy trueInput to the output;
when the select input is false, copy falseInput to the output;
make the output absent when the select input is absent.
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
	name { trueInput }
	type { int }
    }
    input {
	name { falseInput }
	type { int }
    }
    input {
	name { select }
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
	if ( !output.known() && select.known() ) {
	    if ( select.present() ) {
		if ( int(select.get()) ) {

		    // Select is true--copy the true input if it's known

		    if ( trueInput.known() ) {
			if ( trueInput.present() ) {
			    output.emit() << int(trueInput.get());
			} else {
			    // true input is absent: make the output absent
			    output.makeAbsent();
			}
		    }
		} else {

		    // Select is false--copy the false input if it's known

		    if ( falseInput.known() ) {
			if ( falseInput.present() ) {
			    output.emit() << int(trueInput.get());
			} else {
			    // false input is absent: make the output absent
			    output.makeAbsent();
			}
		    }

		}

	    } else {

		// Select is absent: make the output absent

		output.makeAbsent();
	    }		
	}
    }
}
