defstar {
    name { HostASrc }
    domain { CG56 }
    desc { Asynchronous source. }
    version { $Id$ }
    author { Kennard White }
    copyright { 1992 The Regents of the University of California }
    location { CG56 library }
    explanation {
This is a source star (like the Const star).  The value(s) output
by the star may be changed asynchonously by the host via the host port.
This is intended as a base star for derivation; a derived star would provide
some host-specific interface to this star.
.LP
The \fIlabel\fP parameter is not directly used by the star; it is for the
convenience of derived stars.
.SH IMPLEMENTATION
Since we may be followed by an up sampling operation, there may be
an arbitrary number of memory locations cooresponding to our output.
Since we can't expect the host to update all of these values, we
can't apply the trick used by CG56Const to eliminate run-time code
in all cases.  More work must be done to identify the safe cases.
.LP
On each firing of the star, the \fIvalue\fP state is output.
The host may modify the \fIvalue\fP state asyncronously.  Currently
the value state is referenced by explicit memory location; however,
we might want to generate a symbolic name
.LP
Currently only scalors may be output (not waveforms).  This should
be improved.
.LP
Currently derived stars
    }
    seealso { CG56HostSlider, CG56HostButton }
    output {
	    name {output}
	    type { FIX }
    }
    state {
	    name { label }
	    type { STRING }
	    desc { Label for widget on host. }
	    default { "Label" }
    }
    state {
	    name { value }
	    type { FIX }
	    desc { Value to output. }
	    default { 0 }
	    attributes { A_NONCONSTANT|A_NONSETTABLE|A_XMEM }
    }
    codeblock(cbGenericAio) {
generic $ref(value) $fullname() "$val(label)"
    }
    codeblock (cbCopyToOutput) {
	move	$ref(value),x0		; move value to output
	move	x0,$ref(output)
    }

    initCode {
    	addCode(cbGenericAio,"aioCmds");
    }
    go {
    	addCode(cbCopyToOutput);
    }
    exectime {
    	return 1;
    }
}
