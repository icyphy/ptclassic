defstar {
    name { HostButton }
    derivedFrom { HostASrc }
    domain { CG56 }
    desc { Graphical two-valued input source. }
    version { $Id$ }
    author { Kennard White }
    copyright { 1992 The Regents of the University of California }
    location { CG56 library }
    explanation {
.Ir "button"
This is an asynchronous source star (like the Const star) with a particular
graphical user interface.  The star always outputs one of two values;
which value is output is controled by a button.  There are two types
of buttons: \fBpushbutton\fPs and \fBcheckbutton\fPs.
Both presents a single button to the user that
may be "pressed" with the mouse.  The buttons differ
in the semantics of the push.
When the \fBpushbutton\fP is pressed, the \fIonVal\fP state
is output, otherwise \fIoffVal\fP.
The \fBcheckbutton\fP widget is either on or off; pressing it toggles
between on and off.  When on, the \fIonVal\fP state is output,
otherwise \fIoffVal\fP is output.
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
	    type { FIX }
	    desc { Value to output when button off/not pressed. }
	    default { "0" }
    }
    state {
	    name { onVal }
	    type { FIX }
	    desc { Value to output when button on/pressed. }
	    default { ONE }
    }
    state {
	    name { initiallyOn }
	    type { INT }
	    desc { "Boolean: Initial state is on? (checkbutton only)" }
	    default { 0 }
    }
    codeblock(cbButtonAio) {
$val(canonicalWidget) $ref(value) $fullname() {$val(label)} $val(offVal) $val(onVal) $val(initiallyOn)
    }
    start {
	const char *wn = buttonType;
	/*IF*/ if ( strcmp(wn,"pushbutton")==0 
		 || strcmp(wn,"commandbutton")==0 ) {
	    canonicalWidget = "aio_pushbutton";
	    initiallyOn = 0; // param not really used
	    value = double(offVal);
	} else if ( strcmp(wn,"checkbutton")==0 ) {
	    canonicalWidget = "aio_checkbutton";
	    initiallyOn = int(initiallyOn) ? 1 : 0;
	    value = int(initiallyOn) ? double(onVal) : double(offVal);
	} else {
	    Error::abortRun(*this,"Unknown button type.");
	    return;
	}
    }
    initCode {
    	addCode(cbButtonAio,"aioCmds");
    }
}
