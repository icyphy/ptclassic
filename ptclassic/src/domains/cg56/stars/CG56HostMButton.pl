defstar {
    name { HostMButton }
    derivedFrom { HostASrc }
    domain { CG56 }
    desc { Graphical one-of-many input source. }
    version { $Id$ }
    author { Kennard White }
    copyright { 1992 The Regents of the University of California }
    location { CG56 library }
    explanation {
.Ir "button"
This is an asynchronous source star (like the Const star) with a particular
graphical user interface.  The star always outputs one of a finite number
of values: the output is controled by the user selecting one of several
buttons.  Exactly one button in the group is on.
    }
    state {
	    name { pairs }
	    type { STRING }
	    desc { Name and value pairs. }
	    default { "" }
    }
    codeblock(cbMultiButtonAio) {
aio_multibutton $ref(value) $fullname() {$val(label)} {$val(pairs)}
    }
    start {
	// For now, we dont do anycheck of the pairs
    }
    initCode {
    	addCode(cbMultiButtonAio,"aioCmds");
    }
}
