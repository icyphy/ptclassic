defstar {
	name { Pad}
	domain { CG56 }
	desc { Fix pad
On each execution, reads a block of "nread" samples (default 128) and writes
a block of "nwrite" samples (default 256).  The first "offset" samples
(default 0) have value "fill" (default 0.0), the next "nread" outputs
samples have values taken from the inputs, and last "nwrite" -
"nread" - "offset" samples have value "fill" again.
	}
	version { $Id$ }
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990, 1991, 1992 The Regents of the University of California.
All rights reserved.
See the file ~ptolemy/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 control library }
	explanation {
The star pads the input with fill values.
An input block of length \fInread\fR is read in.
The first \fIoffset\fR outputs will be zeros,
then the next \fInread\fR outputs will be the inputs
that are read in, and any remaining outputs will be zeros.
To ensure correct operation,
\fInread\fR + \fIoffset\fR must be smaller than \fInwrite\fR.
}
        seealso { Cut }
	input {
		name {input}
		type {fix}
	}
	output {
		name {output}
		type {fix}
	}
	state {
		name {nread}
		type {int}
		default {128}
		desc {Number of particles read.}
	}
	state {
		name {nwrite}
		type {int}
		default {256}
		desc {Number of particles written.}
	}
	state {
		name {offset}
		type {int}
		default {0}
		desc {Number of leading fill particles in each output block.}
	}
	state {
		name {fill}
		type {float}
		default {0.0}
		desc { Value of fill particles. }
	}

	codeblock (main) {
        move    #<$addr(input),r0
        move    #<$addr(output),r1
	}

        codeblock(off) {
        move    #$val(fill),a
        rep     #$val(offset)
        move    a,x:(r1)+
        }    

        codeblock(read) {
        do      #$val(nread),$label(rloop)
        move    x:(r0)+,a
        move    a,x:(r1)+
$label(rloop)
        }

        codeblock(write) {
        move    #$val(fill),a
        rep     #$val(nwrite)-$val(nread)-$val(offset)
        move    a,x:(r1)+
        }

	setup {
		input.setSDFParams(int (nread), int (nread) - 1);
		output.setSDFParams(int (nwrite), int (nwrite) - 1);

		if (nread + offset > nwrite) {
		Error::	abortRun(*this, ": nread + offset > nwrite.");
			return;
		}
	}
	go {
		addCode(main);
		if (offset > 0)
			addCode(off);
		if (nread > 0)
			addCode(read);
		if (nwrite > nread + offset)
			addCode(write);
	}
	execTime {
		int             a, b, c;

		if (offset > 0)
			a = 2 + int (offset);
		if (nread > 0)
			b = 2 + 2 * int (nread);
		if (nwrite > nread + offset)
			c = 2 + int (nwrite) - int (nread) - int (offset);
		return 4 + a + b + c;
	}
}
