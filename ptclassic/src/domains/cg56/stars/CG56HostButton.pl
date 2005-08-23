defstar {
    name { HostButton }
    derivedFrom { HostASrc }
    domain { CG56 }
    desc { Graphical two-valued input source. }
    version { @(#)CG56HostButton.pl	1.16 03/29/97 }
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
This is an asynchronous source star (like the Const star) with a particular
graphical user interface.
The star always outputs one of two values; which value is output is
controlled by a button.
There are two types of buttons: <b>pushbutton</b>s and <b>checkbutton</b>s.
Both presents a single button to the user that may be "pressed" with the mouse.
The buttons differ in the semantics of the push.
When the <b>pushbutton</b> is pressed, the <i>onVal</i> state
is output, otherwise <i>offVal</i>.
The <b>checkbutton</b> widget is either on or off; pressing it toggles
between on and off.
When on, the <i>onVal</i> state is output; otherwise <i>offVal</i> is output.
<p>
The <i>offVal</i> and <i>onVal</i> should be either FIX or INT type.
They are not examined at compile-time: they are passed literally to qdm
(via the aio file) and to the assembler (via the initial value).
Note that there is a big difference between "1" and "1.0".
    }
    state {
	name { buttonType }
	type { STRING }
	desc { Type of button: pushbutton, checkbutton. }
	default { "pushbutton" }
    }
    state {
	name { canonicalWidget }
	type { STRING }
	desc { Canonical form of widget name. }
	default { "xxx" }
	attributes { A_NONSETTABLE }
    }
    state {
	name { offVal }
	type { STRING }
	desc { "Value to output when button off/not pressed (either FIX or INT)." }
	default { "0.0" }
    }
    state {
	name { onVal }
	type { STRING }
	desc { "Value to output when button on/pressed (either FIX or INT)." }
	default { "1.0" }
    }
    state {
	name { initiallyOn }
	type { INT }
	desc { "Boolean: Initial state is on? (checkbutton only)" }
	default { 0 }
    }
    state {
	name { initValStr }
	type { STRING }
	desc { "String form of initial value." }
	default { "0" }
	attributes { A_NONSETTABLE }
    }
    codeblock(cbButtonAio) {
$val(canonicalWidget) $ref(value) $fullname() {$val(label)} $val(offVal) $val(onVal) $val(initiallyOn)
    }
    codeblock(cbInitValue) {
	org	$ref(value)
	dc	$val(initValStr)
	org	p:
    }
    setup {
	const char *wn = buttonType;
	/*IF*/ if ( strcmp(wn,"pushbutton")==0 
		 || strcmp(wn,"commandbutton")==0 ) {
	    canonicalWidget = "aio_pushbutton";
	    initiallyOn = 0; // param not really used
	    initValStr = (const char*) offVal;
	} else if ( strcmp(wn,"checkbutton")==0 ) {
	    canonicalWidget = "aio_checkbutton";
	    initiallyOn = int(initiallyOn) ? 1 : 0;
	    initValStr = int(initiallyOn) 
	      ? (const char*) onVal : (const char*) offVal;
	} else {
	    Error::abortRun(*this,"Unknown button type.");
	    return;
	}
	value.setAttributes(A_NOINIT);
    }
    initCode {
    	addCode(cbButtonAio,"aioCmds");
    	addCode(cbInitValue);
    }
}
