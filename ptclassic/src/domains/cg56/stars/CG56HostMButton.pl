defstar {
    name { HostMButton }
    derivedFrom { HostASrc }
    domain { CG56 }
    desc { Graphical one-of-many input source. }
    version { $Id$ }
    author { Kennard White }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
    location { CG56 io library }
    explanation {
.Ir "button"
This is an asynchronous source star (like the Const star) with a particular
graphical user interface.  The star always outputs one of a finite number
of values: the output is controled by the user selecting one of several
buttons.  Exactly one button in the group is on.
.LP
The \fIpairs\fP parameter defines a set of pairs.  Each pair should be
enclosed in double-quotes.  Each pair consists of two words: a one word
label and a value.  The value most be either a FIX or INT.  The value
is not checked at compile time: it is literally passed to qdm.
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
