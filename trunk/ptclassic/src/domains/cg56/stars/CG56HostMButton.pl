defstar {
    name { HostMButton }
    derivedFrom { HostASrc }
    domain { CG56 }
    desc { Graphical one-of-many input source. }
    version { @(#)CG56HostMButton.pl	1.14 03/29/97 }
    author { Kennard White }
    copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }
    htmldoc {
<a name="button"></a>
This is an asynchronous source star (like the
<tt>Const</tt>
star) with a particular graphical user interface.
The star always outputs one of a finite number of values: the output is
controlled by the user selecting one of several buttons.
Exactly one button in the group is on.
<p>
The <i>pairs</i> parameter defines a set of pairs.
Each pair should be enclosed in double quotes.
Each pair consists of two words: a one word label and a value.
The value most be either a FIX or INT.
The value is not checked at compile time: it is literally passed to qdm.
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
    setup {
	// For now, we don't do any checking of the pairs
    }
    initCode {
    	addCode(cbMultiButtonAio,"aioCmds");
    }
}
