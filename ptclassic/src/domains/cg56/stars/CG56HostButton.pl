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
This is a source star (like the Const star).  The value(s) output
by the star may be changed asynchonously by the host via the host port.
When used in conjuction with a graphical target monitor (.e.g., qdm_s56x),
a graphical user interface under X windows is provided to control the
source value.
.LP
Currently only scalors may be output (not waveforms).  One of three
graphical interfaces may be used: pushbutton, checkbutton, and slider.
(scale is synonymous with slider, commandbutton is synonymous with pushbutton).
The \fIbuttonType\fP state controls which interface will be used.
.LP
Both \fBbutton\fP widgets presents a single button to the user that
may be "pressed" with the mouse.  The buttons differ
in the semantics of the push.
When the \fBpushbutton\fP is pressed, the \fIdspMaxVal\fP state
is output, otherwise \fIdspMinVal\fP.
The \fBcheckbutton\fP widget is either on or off; pressing it toggles
between on and off.  When on, the \fIdspMaxVal\fP state is output,
otherwise \fIdspMinVal\fP is output.
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
$val(canonicalWidget) $ref(value) $fullname() "$val(label)" $val(offVal) $val(onVal) $val(initiallyOn)
    }
    start {
	const char *wn = buttonType;
	/*IF*/ if ( strcmp(wn,"pushbutton")==0 
		 || strcmp(wn,"commandbutton")==0 ) {
	    canonicalWidget = "aio_pushbutton";
	    initiallyOn = 0;
	} else if ( strcmp(wn,"checkbutton")==0 ) {
	    canonicalWidget = "aio_checkbutton";
	    initiallyOn = int(initiallyOn) ? 1 : 0;
	} else {
	    Error::abortRun(*this,"Unknown button type.");
	    return;
	}
    }
    initCode {
    	addCode(cbButtonAio,"aioCmds");
    }
}
