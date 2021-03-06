defstar {
	name {Cut}
	domain {CG56}
	desc {
On each execution, this star reads a block of "nread" samples (default 128)
and writes "nwrite" of these samples (default 64), skipping the first
"offset" samples (default 0).  It is an error if nwrite + offset > nread.
If nwrite > nread, then the output consists of overlapping windows,
and hence "offset" must be negative.
	}
	version {$Id$ }
	author { Chih-Tsung Huang }
	copyright {
Copyright (c) 1990-%Q% The Regents of the University of California.
All rights reserved.
See the file $PTOLEMY/copyright for copyright notice,
limitation of liability, and disclaimer of warranty provisions.
	}
	location { CG56 control library }
	explanation {
This star reads a block of particles of any type, and writes a block
of particles that somehow overlaps the input block.
The number of input particles consumed is given by \fInread\fR,
and the number of output particles produced is given by \fInwrite\fR.
The \fIoffset\fR parameter (default 0) specifies where the output
block should start, relative to the beginning of the input block.
To avoid trying to read samples that have not yet been consumed,
it is required that $nwrite ~+~ offset ~<=~ nread$.
Hence, if $nwrite ~>~ nread$, \fIoffset\fR must be negative,
and the output consists of overlapping blocks input particles.
	}
	input {
		name{input}
		type{ANYTYPE}
	}
	output {
		name{output}
		type{=input}
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
		default {64}
		desc {Number of particles written.}
	}
	state {
		name {offset}
		type {int}
		default {0}
		desc { Position of output block relative to input block.}
	}
	setup {
		if (int(nwrite) + int(offset) > int(nread)) {
			Error::abortRun(*this,
			   "nwrite + offset cannot be greater than nread");
			return;
		}
		input.setSDFParams(int(nread),int(nread)-int(offset)-1);
		output.setSDFParams(int(nwrite),int(nwrite)-1);
	}
        codeblock(main) {
        move    #$addr(input,offset),r0
        move    #$addr(output),r1
        }
        codeblock(write) {
        do      #$val(nwrite),$label(loop)
        move    $mem(input):(r0)+,a
        move    $mem(output),x:(r1)+
$label(loop)
        }    
	go {
                addCode(main);
		if(int(nwrite) > 0) addCode(write);
	}
        execTime {
                return 4 + 2*int(nwrite);
        }

}
