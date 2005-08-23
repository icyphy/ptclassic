defstar {
    name { HostASrc }
    domain { CG56 }
    desc { Asynchronous source. }
    version { @(#)CG56HostASrc.pl	1.14 03/29/97 }
    author { Kennard White }
    copyright {
Copyright (c) 1990-1997 The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
    }
    location { CG56 main library }
    htmldoc {
This is a source star (like the
<tt>Const</tt>
star).
The value(s) output by the star may be changed asynchronously by
the host via the host port.
This is intended as a base star for derivation; a derived star would provide
some host-specific interface to this star.
The <i>label</i> parameter is not directly used by the star; it is for the
convenience of derived stars.
<h3>IMPLEMENTATION</h3>
<p>
Since we may be followed by an up sampling operation, there may be
an arbitrary number of memory locations corresponding to our output.
Since we cannot expect the host to update all of these values, we
cannot apply the trick used by the
<tt>Const</tt>
star to eliminate runtime code in all cases.
More work must be done to identify the safe cases.
<p>
On each firing of the star, the <i>value</i> state is output.
The host may modify the <i>value</i> state asynchronously.
Currently, the value state is referenced by explicit memory location; however,
we might want to generate a symbolic name.
Currently, only scalars may be output (not waveforms).
This should be improved.
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
aio_generic $ref(value) $fullname() "$val(label)"
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
